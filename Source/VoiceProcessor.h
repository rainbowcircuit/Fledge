/*
  ==============================================================================

    VoiceProcessor.h
    Created: 19 May 2025 2:49:25pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Operator.h"
#include "AlgorithmHelper.h"

class Synthesizer : public juce::Synthesiser
{
public:
    void noteOn(int midiChannel, int midiNoteNumber, float velocity) override
    {
        const juce::ScopedLock sl(lock);
        
        for (auto* sound : sounds)
        {
            if (sound->appliesToNote(midiNoteNumber) && sound->appliesToChannel(midiChannel))
            {
                // Round robin voice allocation
                auto* voice = findVoiceRoundRobin(sound);
                if (voice != nullptr)
                {
                    startVoice(voice, sound, midiChannel, midiNoteNumber, velocity);
                }
                break;
            }
        }
    }
    
    void setVoiceCount(int voiceCount)
    {
        this->voiceCount = voiceCount;
    }
    
private:
    
    juce::SynthesiserVoice* findVoiceRoundRobin(juce::SynthesiserSound* soundToPlay)
    {
        for (int attempts = 0; attempts < voiceCount; attempts++)
        {
            auto* voice = voices[nextVoiceIndex];
            nextVoiceIndex = (nextVoiceIndex + 1) % voiceCount;
            
            if (voice->canPlaySound(soundToPlay))
            {
                if(voice->isVoiceActive()) voice->stopNote(1.0f, true);
                return voice;
            }
        }
        return nullptr;
    }

    int nextVoiceIndex = 0;
    int voiceCount = 16;
};

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return 1; }
    bool appliesToChannel(int midiChannel) override { return 1; }
};

class SynthVoice : public juce::SynthesiserVoice, AlgorithmHelper
{
public:
    void prepareToPlay(double sampleRate, float samplesPerBlock, int numChannels);
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    
    //==============================================================================
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    
    //==============================================================================
    void setEnvelope(int index, float attack, float decay, float sustain, float release, float globalAttack, float globalDecay, float globalSustain, float globalRelease);
    void setFMParameters(int index, float ratio, float fixed, bool isFixed, float amplitude, float phase, float globalModIndex, float gainInDecibel);
    
    //==============================================================================
    void pitchWheelMoved(int newPitchWheelValue) override 
    {
        for (int i = 0; i < 4; i++)
        {
            op[i].setPitchbend(newPitchWheelValue);
        }
    }
    void channelPressureChanged (int newChannelPressureValue) override
    {
        channelPressureFloat = newChannelPressureValue / 127.0f;
        for (int i = 0; i < 4; i++)
        {
            op[i].setPressure(channelPressureFloat);
        }
    }
    void controllerMoved(int controllerNumber, int newControllerValue) override {}
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    
    //==============================================================================
    void setOperatorGain(int index, int gainIndex, int outputGainIndex);
    float getOutputSample();
    
private:
    double sampleRate;
    juce::AudioBuffer<float> synthBuffer;
    float outputSample;
    float channelPressureFloat;
    float gainInAmp;
    float op0 = 0.0f, op1 = 0.0f, op2 = 0.0f, op3 = 0.0f, feedback = 0.0f; // unit delays for algorithm
    
    std::array<float, 4> op3Gain = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 4> op2Gain = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 4> op1Gain = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 4> op0Gain = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 4> outputGain = { 0.0f, 0.0f, 0.0f, 0.0f };

    std::array<FMOperator, 4> op;
    
    float attackScaled;
    float decayScaled;  
    float sustainScaled;
    float releaseScaled;
    float output;

};

class MidiProcessor : public juce::MidiMessage
{
public:
    float getMidiFrequencyCents(float midiNoteCents)
    {
        return std::pow(2, (midiNoteCents-6900.0f)/1200.0f) * 440.0f;
    }
};
