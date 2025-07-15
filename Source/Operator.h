

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
    float processOperator(float phase1, float phase2, float phase3, float phase4);
    
private:
    double sampleRate;
    double operatorPhase = 0.0, operatorAngle = 0.0;
    double prevInputSum = 0.0;
    float modulationIndex = 1.0f;
    float noteFrequency, frequency, ratio, fixed;
    bool isFixed = false;
    
    juce::SmoothedValue<float> ratioSmoothed, fixedSmoothed, modIndexSmoothed;
    juce::ADSR ampEnvelope;
    juce::ADSR::Parameters envParameters;
};
