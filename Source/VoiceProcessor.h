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
        
        setOperatorGain(); // move this elsewhere
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
        setOperatorGain(); // move elsewehre later
        for (int sample = 0; sample < outputBuffer.getNumSamples(); ++sample) {
            op3 = op[3].processOperator(op0 * op3Gain[0],
                                        op1 * op3Gain[1],
                                        op2 * op3Gain[2],
                                        op3 * op3Gain[3],
                                        feedback * op3Gain[4]);
            
            op2 = op[2].processOperator(op0 * op2Gain[0],
                                        op1 * op2Gain[1],
                                        op2 * op2Gain[2],
                                        op3 * op2Gain[3],
                                        feedback * op2Gain[4]);

            op1 = op[1].processOperator(op0 * op1Gain[0],
                                        op1 * op1Gain[1],
                                        op2 * op1Gain[2],
                                        op3 * op1Gain[3],
                                        feedback * op1Gain[4]);

            op0 = op[0].processOperator(op0 * op0Gain[0],
                                        op1 * op0Gain[1],
                                        op2 * op0Gain[2],
                                        op3 * op0Gain[3],
                                        feedback * op0Gain[4]);

            float output = op0 * outputGain[0] +
                           op1 * outputGain[1] +
                           op2 * outputGain[2] +
                           op3 * outputGain[3] +
                           feedback * outputGain[4];

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
                outputBuffer.setSample(channel, sample, output);
                
            }
        }
    }
    
    void setOperatorGain()
    {
        // potentially replace with a different data structure than an array?
        op3Gain = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }; // no feedback atm
        op2Gain = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };
        op1Gain = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
        op0Gain = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
        outputGain = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f }; // op1, op2, op3, op4, feedback
        
    }
    
    void createWavetable() // maybe unused atm
    {
        /*
        outputWavetable.setSize(1, (int) tableSize);
        auto* samples = outputWavetable.getWritePointer(0);
        
        for (unsigned int i = 0; i < tableSize; ++i)
        {
            setOperatorGain();
            op3 = op[3].processOperator(op0 * op1Gain[0],
                                        op1 * op1Gain[1],
                                        op2 * op1Gain[2],
                                        op3 * op1Gain[3],
                                        feedback * op1Gain[4]);
            
            op2 = op[2].processOperator(op0 * op1Gain[0],
                                        op1 * op1Gain[1],
                                        op2 * op1Gain[2],
                                        op3 * op1Gain[3],
                                        feedback * op1Gain[4]);

            op1 = op[1].processOperator(op0 * op1Gain[0],
                                        op1 * op1Gain[1],
                                        op2 * op1Gain[2],
                                        op3 * op1Gain[3],
                                        feedback * op1Gain[4]);

            op0 = op[0].processOperator(op0 * op1Gain[0],
                                        op1 * op1Gain[1],
                                        op2 * op1Gain[2],
                                        op3 * op1Gain[3],
                                        feedback * op1Gain[4]);

            float output = op0 * outputGain[0] +
                           op1 * outputGain[1] +
                           op2 * outputGain[2] +
                           op3 * outputGain[3] +
                           feedback * outputGain[4];

            samples[i] = output;
        }
         */

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
    float op0 = 0.0f, op1 = 0.0f, op2 = 0.0f, op3 = 0.0f, feedback = 0.0f; // unit delays for algorithm
    
    std::array<float, 5> op3Gain = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 5> op2Gain = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 5> op1Gain = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 5> op0Gain = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 5> outputGain = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

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

