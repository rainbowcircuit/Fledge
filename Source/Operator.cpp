
#include "Operator.h"
#include "VoiceProcessor.h"

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

MidiProcessor midi;

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
    noteNumberCents = noteNumber * 100.0f;
    setPlayedFrequency();
}
void FMOperator::setPitchbend(int pitchWheelPosition)
{
    float pitchWheelFloat = pitchWheelPosition * 1.0f;
    pitchWheelCents = juce::jmap(pitchWheelFloat, 0.0f, 16383.0f, -4800.0f, 4800.0f);
    setPlayedFrequency();
}
void FMOperator::setPlayedFrequency()
{
    playedFrequency = midi.getMidiFrequencyCents(noteNumberCents + pitchWheelCents);
    noteFrequencySmoothed.setTargetValue(playedFrequency);
}
void FMOperator::setVelocity(float velocity)
{
    noteVelocity = std::pow(velocity, 2);
    noteVelocitySmoothed.setTargetValue(noteVelocity);
}
void FMOperator::setPressure(float pressure)
{   
    channelPressure = (std::tanh(pressure * juce::MathConstants<float>::pi) * 0.25f) + 0.75f;//this still produces some truncation distortion, needs reworking, 7 bits just isnt alot of resolution
    channelPressureSmoothed.setTargetValue(channelPressure);
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
    
    frequency = noteFrequencySmoothed.getNextValue() * ratioSmoothed.getNextValue();
    if (isFixed) frequency = fixedSmoothed.getNextValue();
    operatorAngle = frequency/sampleRate;
    
    float modulatorPhase = phase1 + phase2 + phase3 + phase4;
    float twopi = juce::MathConstants<float>::twoPi;
    float envelope = ampEnvelope.getNextSample() * noteVelocitySmoothed.getNextValue() * channelPressureSmoothed.getNextValue();//multiple amp envelope by velocity
    float phaseOffset = phaseSmoothed.getNextValue();
    float modIndex = std::pow(2.0f, globalModIndexSmoothed.getNextValue() / 100.0f) * 8.0f;
    float waveform = std::sin((operatorPhase + phaseOffset) * twopi + (modulatorPhase * modIndex)) * envelope; // 8 is the mod index

    // accumulate and wrap
    operatorPhase += operatorAngle;
    if (operatorPhase >= 1.0) operatorPhase -= 1.0;

    return waveform * amplitudeSmoothed.getNextValue();
}
