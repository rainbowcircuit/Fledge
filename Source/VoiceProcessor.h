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
    float processOperator(float modulatorPhase);
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
    }
    void stopNote(float velocity, bool allowTailOff) override
    {
        for (int i = 0; i < 4; i++)
        {
            op[i].stopNote();
        }
    }
    
    void setEnvelope(int index, float attack, float decay, float sustain, float release)
    {
        op[index].setEnvelope(attack, decay, sustain, release, false);
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
            float operator4 = op[3].processOperator(0.0);
            float operator3 = op[2].processOperator(operator4);
            float operator2 = op[1].processOperator(operator3);
            float operator1 = op[0].processOperator(operator2);
            
            float output = operator1 * 0.5f;

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
                outputBuffer.setSample(channel, sample, output);
                
            }
        }
/*
        
        
        for(int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            auto* channelData = outputBuffer.getWritePointer(channel);
            
            for(int sample = 0; sample < numSamples; ++sample)
            {
                float operator4 = op[3].processOperator(0.0);
                float operator3 = op[2].processOperator(operator4);
                float operator2 = op[1].processOperator(operator3);
                float operator1 = op[0].processOperator(operator2);
                
                channelData[sample] = operator1 * 0.5f;
            }
        }
 */
}

private:
    std::array<FMOperator, 4> op;
    
};

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return 1; }
    bool appliesToChannel(int midiChannel) override { return 1; }
};

