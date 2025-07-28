
#include "Operator.h"

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
    
    ratioSmoothed.reset(sampleRate, 0.001);
    fixedSmoothed.reset(sampleRate, 0.001);
    amplitudeSmoothed.reset(sampleRate, 0.001);
}

void FMOperator::startNote()
{
    ampEnvelope.noteOn();
    operatorAngle = 0.0;
    operatorPhase = 0.0;
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
    ratioSmoothed.setTargetValue(ratio);
    fixedSmoothed.setTargetValue(fixed);
    amplitudeSmoothed.setTargetValue(modIndex/100.0f);
    this->isFixed = isFixed;
}

float FMOperator::processOperator(float phase1, float phase2, float phase3, float phase4)
{
    frequency = noteFrequency * ratioSmoothed.getNextValue();
    if (isFixed) frequency = fixedSmoothed.getNextValue();
    operatorAngle = frequency/sampleRate;
    
    float modulatorPhase = phase1 + phase2 + phase3 + phase4;
    float twopi = juce::MathConstants<float>::twoPi;
    float envelope = ampEnvelope.getNextSample();
    float waveform = std::sin(operatorPhase * twopi + (modulatorPhase * 8.0f)) * envelope; // 8 is the mod index

    // accumulate and wrap
    operatorPhase += operatorAngle;
    if (operatorPhase >= 1.0) operatorPhase -= 1.0;

    return waveform * amplitudeSmoothed.getNextValue();
}
