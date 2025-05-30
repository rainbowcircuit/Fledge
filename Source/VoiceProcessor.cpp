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

void FMOperator::setNoteNumber(float noteNumber)
{
    noteFrequency = juce::MidiMessage::getMidiNoteInHertz(noteNumber);
}

void FMOperator::setOperator(float ratio, float fixed, bool isFixed, float modIndex)
{
    this->ratio = ratio;
    this->fixed = fixed;
    this->isFixed = isFixed;
    modulationIndex = modIndex;
}

float FMOperator::processOperator(float modulatorPhase)
{
    frequency = noteFrequency * ratio;
    if (isFixed) frequency = fixed;
    operatorAngle = frequency/sampleRate;

    float twopi = juce::MathConstants<float>::pi * 2.0f;
    float envelope = ampEnvelope.getNextSample();
    float waveform = std::sin(operatorPhase * twopi + (modulatorPhase * modulationIndex)) * envelope;
    
    // accumulate and wrap
    operatorPhase += operatorAngle;
    if (operatorPhase >= 1.0) operatorPhase -= 1.0;
    
    return waveform;
}

void FMOperator::setFMInputs(float phaseIn1, float phaseIn2, float phaseIn3, float phaseInAmplitude1, float phaseInAmplitude2, float phaseInAmplitude3)
{
    float scaled1 = phaseIn1 * phaseInAmplitude1;
    float scaled2 = phaseIn2 * phaseInAmplitude2;
    float scaled3 = phaseIn3 * phaseInAmplitude3;

    float fmOutput = scaled1 + scaled2 + scaled3 + frequency;
}
