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
    void setFMParameters(int index, float ratio, float fixed, bool isFixed, float amplitude, float phase, float globalModIndex);
    
    //==============================================================================
    void pitchWheelMoved(int newPitchWheelValue) override {}
    void controllerMoved(int controllerNumber, int newControllerValue) override {}
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    
    //==============================================================================
    void setOperatorGain(int index, int gainIndex, int outputGainIndex);
    float getOutputSample();
    
private:
    double sampleRate;
    juce::AudioBuffer<float> synthBuffer;
    float outputSample;
    
    float op0 = 0.0f, op1 = 0.0f, op2 = 0.0f, op3 = 0.0f, feedback = 0.0f; // unit delays for algorithm
    
    std::array<float, 4> op3Gain = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 4> op2Gain = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 4> op1Gain = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 4> op0Gain = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<float, 4> outputGain = { 0.0f, 0.0f, 0.0f, 0.0f };

    std::array<FMOperator, 4> op;
};

