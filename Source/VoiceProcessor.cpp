/*
  ==============================================================================

    VoiceProcessor.cpp
    Created: 19 May 2025 2:49:25pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "VoiceProcessor.h"
#include <JuceHeader.h>


void FMOperator::prepareToPlay(double sampleRate, float samplesPerBlock, int numChannels)
{
    this->sampleRate = sampleRate;
    
    // envelope
    ampEnvelope.setSampleRate(sampleRate);
    ampEnvelope.reset();

    envParameters.attack = 1000.0f;
    envParameters.decay = 250.0f;
    envParameters.sustain = 0.8f;
    envParameters.release = 1000.0f;

    ampEnvelope.setParameters(envParameters);
}

void FMOperator::startNote()
{
    ampEnvelope.noteOn();
}

void FMOperator::stopNote()
{
    ampEnvelope.noteOff();
}

void FMOperator::setEnvelope(float attackInMs, float decayInMs, float sustainInFloat, float releaseInMs, bool isLooping)
{
    envParameters.attack = attackInMs;
    envParameters.decay = decayInMs;
    envParameters.sustain = sustainInFloat;
    envParameters.release = releaseInMs;
    
    ampEnvelope.setParameters(envParameters);
}

void FMOperator::setOperator(float frequencyInHz, float ratio, float modulationIndex)
{
    float frequency = frequencyInHz * ratio;
    operatorAngle = frequency/sampleRate;

    this->modulationIndex = modulationIndex;
}

float FMOperator::processOperator(float modulatorPhase)
{
    float twopi = juce::MathConstants<float>::pi * 2;
    float envelope = ampEnvelope.getNextSample();
    float waveform = std::sin(operatorPhase * twopi + (modulatorPhase * modulationIndex)) * envelope;
    
    // accumulate and wrap
    operatorPhase += operatorAngle;
    if (operatorPhase >= 1.0) operatorPhase -= 1.0;
    
    return waveform;
}

