

#pragma once
#include <JuceHeader.h>
#include "ADSREnvelope.h"

class FMOperator
{
public:
    void prepareToPlay(double sampleRate, float samplesPerBlock, int numChannels);
    void startNote();
    void stopNote();
    
    void setEnvelope(float attack, float decay, float sustain, float release, bool isLooping);
    void setNoteNumber(float noteNumber);
    void setPitchbend(int PitchWheelPostion);
    void setPlayedFrequency();
    void setVelocity(float velocity);
    void setPressure(float pressure);
    void setOperator(float ratio, float fixed, bool isFixed, float amplitude, float phase, float globalModIndex);
    float processOperator(float phase1, float phase2, float phase3, float phase4);
    
    ADSREnvelope ampEnvelope;
    juce::ADSR::Parameters envParameters;
private:
    double sampleRate;
    double operatorPhase = 0.0, operatorAngle = 0.0;
    double prevInputSum = 0.0;
    float modulationIndex = 1.0f;
    float playedFrequency, noteNumberCents, frequency, ratio, fixed, noteVelocity;
    float channelPressure = 1.0f;
    float pitchWheelCents = 0.0f;
    bool isFixed = false;
    
    float targetFrequency = 0.0f;
    float currentFrequency = 0.0f;
    float frequencySmoothingTimeMs = 100.0f;
    float frequencySmoothingCoeff = 0.0f;
    
    
    juce::SmoothedValue<float> ratioSmoothed, fixedSmoothed, noteFrequencySmoothed, amplitudeSmoothed, phaseSmoothed, sustainSmoothed, globalModIndexSmoothed, noteVelocitySmoothed, channelPressureSmoothed;
};
