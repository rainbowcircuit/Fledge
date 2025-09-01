
#include "Operator.h"

void FMOperator::prepareToPlay(double sampleRate, float samplesPerBlock, int numChannels)
{
    this->sampleRate = sampleRate;
    
    ampEnvelope.setSampleRate(sampleRate);
    
    ratioSmoothed.reset(sampleRate, 0.001);
    fixedSmoothed.reset(sampleRate, 0.001);
    amplitudeSmoothed.reset(sampleRate, 0.001);
    phaseSmoothed.reset(sampleRate, 0.001);
    sustainSmoothed.reset(sampleRate, 0.001);
    globalModIndexSmoothed.reset(sampleRate, 0.001);
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
    ampEnvelope.setEnvelopeParameters(attackInMs, decayInMs, sustainInFloat, releaseInMs);
}

void FMOperator::setNoteNumber(float noteNumber)
{
    noteFrequency = juce::MidiMessage::getMidiNoteInHertz(noteNumber);
}

void FMOperator::setOperator(float ratio, float fixed, bool isFixed, float amplitude, float phase, float globalModIndex)
{
    ratioSmoothed.setTargetValue(ratio);
    fixedSmoothed.setTargetValue(fixed);
    amplitudeSmoothed.setTargetValue(amplitude/100.0f);
    phaseSmoothed.setTargetValue(phase/100.0f);
    globalModIndexSmoothed.setTargetValue(globalModIndex);
    this->isFixed = isFixed;
}

float FMOperator::processOperator(float phase1, float phase2, float phase3, float phase4)
{
    envParameters.sustain = sustainSmoothed.getNextValue();
   // ampEnvelope.setParameters(envParameters);
    
    frequency = noteFrequency * ratioSmoothed.getNextValue();
    if (isFixed) frequency = fixedSmoothed.getNextValue();
    operatorAngle = frequency/sampleRate;
    
    float modulatorPhase = phase1 + phase2 + phase3 + phase4;
    float twopi = juce::MathConstants<float>::twoPi;
    float envelope = ampEnvelope.getNextSample();
    float phaseOffset = phaseSmoothed.getNextValue();
    float modIndex = std::pow(2.0f, globalModIndexSmoothed.getNextValue() / 100.0f) * 8.0f;
    float waveform = std::sin((operatorPhase + phaseOffset) * twopi + (modulatorPhase * modIndex)) * envelope; // 8 is the mod index

    // accumulate and wrap
    operatorPhase += operatorAngle;
    if (operatorPhase >= 1.0) operatorPhase -= 1.0;

    return waveform * amplitudeSmoothed.getNextValue();
}
