/*
  ==============================================================================

    VoiceProcessor.h
    Created: 19 May 2025 2:49:25pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FMOperator
{
public:
    void prepareToPlay(double sampleRate, float samplesPerBlock, int numChannels);
    void startNote();
    void stopNote();
    
    void setEnvelope(float attack, float decay, float sustain, float release, bool isLooping);
    void setNoteNumber(float noteNumber);
    void setOperator(float ratio, float fixed, bool isFixed, float modIndex);
    float processOperator(float modulatorPhase1, float modulatorPhase2, float modulatorPhase3, float modulatorPhase4);
    void setFMInputs(float phaseIn1, float phaseIn2, float phaseIn3, float phaseInAmplitude1, float phaseInAmplitude2, float phaseInAmplitude3);
    
private:
    double sampleRate;
    double operatorPhase = 0.0, operatorAngle = 0.0;
    double prevInputSum = 0.0;
    float modulationIndex = 1.0f;
    float noteFrequency, frequency, ratio, fixed;
    bool isFixed = false;
    
    juce::ADSR ampEnvelope;
    juce::ADSR::Parameters envParameters;
};


class SynthVoice : public juce::SynthesiserVoice
{
public:
    void prepareToPlay(double sampleRate, float samplesPerBlock, int numChannels)
    {
        this->sampleRate = sampleRate;
        for (int i = 0; i < 4; i++)
        {
            op[i].prepareToPlay(sampleRate, samplesPerBlock, numChannels);
        }
    }
    
    bool canPlaySound(juce::SynthesiserSound *) override { return 1; }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override
    {
        for (int i = 0; i < 4; i++)
        {
            op[i].startNote();
            op[i].setNoteNumber(midiNoteNumber);
        }
        float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        
        auto tableSizeOverSampleRate = (float)outputWavetable.getNumSamples()/sampleRate;
        tableDelta = frequency * tableSizeOverSampleRate;
    }
    
    void stopNote(float velocity, bool allowTailOff) override
    {
        for (int i = 0; i < 4; i++)
        {
            op[i].stopNote();
        }
    }
    
    void setEnvelope(int index, float attack, float decay, float sustain, float release, float globalAttack, float globalDecay, float globalSustain, float globalRelease)
    {
        float attackScaled = std::pow(2.0f, globalAttack / 100.0f) * attack;
        float decayScaled = std::pow(2.0f, globalDecay / 100.0f) * decay;
        float sustainScaled = std::pow(2.0f, globalSustain / 100.0f) * sustain;
        sustainScaled = juce::jlimit(0.0f, 1.0f, sustainScaled);
        float releaseScaled = std::pow(2.0f, globalRelease / 100.0f) * release;

        op[index].setEnvelope(attackScaled,
                              decayScaled,
                              sustainScaled,
                              releaseScaled, false);
    }
    
    void setFMParameters(int index, float ratio, float fixed, bool isFixed, float modIndex)
    {
        op[index].setOperator(ratio, fixed, isFixed, modIndex);
    }
    
    
    void pitchWheelMoved(int newPitchWheelValue) override {}
    void controllerMoved(int controllerNumber, int newControllerValue) override {}
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override
    {
        for (int sample = 0; sample < outputBuffer.getNumSamples(); ++sample) {
            op4 = op[3].processOperator(op1, op2, op3, op4);
            op3 = op[2].processOperator(op1, op2, op3, op4);
            op2 = op[1].processOperator(op1, op2, op3, op4);
            op1 = op[0].processOperator(op1, op2, op3, op4);
            
            float output = op1 * 0.5f;

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
                outputBuffer.setSample(channel, sample, output);
                
            }
        }
    }
    
    void createWavetable()
    {
        outputWavetable.setSize(1, (int) tableSize);
        auto* samples = outputWavetable.getWritePointer(0);
        
        for (unsigned int i = 0; i < tableSize; ++i)
        {
            op4 = op[3].processOperator(op1, op2, op3, op4);
            op3 = op[2].processOperator(op1, op2, op3, op4);
            op2 = op[1].processOperator(op1, op2, op3, op4);
            op1 = op[0].processOperator(op1, op2, op3, op4);
            
            float output = op1 * 0.5f;

            samples[i] = output;
        }
    }
    
    forcedinline float getNextSample()
    {
        auto tableSize = (unsigned int)outputWavetable.getNumSamples();
        auto index0 = (unsigned int) currentIndex;
        auto index1 = index0 == (tableSize - 1) ? 0 : index0 + 1;
        auto frac = currentIndex - (float) index0;
        
        auto* table = outputWavetable.getReadPointer(0);
        auto value0 = table[index0];
        auto value1 = table[index1];

        auto currentSample = value0 + frac * (value1 - value0);
        if ((currentIndex += tableDelta) > (float)tableSize)
        {
            currentIndex -= (float)tableSize;
        }
        
        return currentSample;
    }

private:
    double sampleRate;
    float op1 = 0.0f, op2 = 0.0f, op3 = 0.0f, op4 = 0.0f; // unit delays for algorithm
    std::array<FMOperator, 4> op;
    juce::AudioSampleBuffer outputWavetable;
    int tableSize = 128;
    float tableDelta, currentIndex, tableSizeOverSampleRate;
};

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return 1; }
    bool appliesToChannel(int midiChannel) override { return 1; }
};

