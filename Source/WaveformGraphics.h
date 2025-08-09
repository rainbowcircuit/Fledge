/*
  ==============================================================================

    WaveformGraphics.h
    Created: 8 Aug 2025 10:45:15pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AlgorithmHelper.h"
#include "LookAndFeel.h"

class WaveformDisplayGraphics : public juce::Component, AlgorithmHelper
{
public:
    WaveformDisplayGraphics();
    
    void paint(juce::Graphics &g) override;
    void resized() override {};

    void setGainCoefficients(int index, int gainIndex, int outputGainIndex);
    void setEnvelope(int index, float attack, float decay, float sustain, float release);
    void setFMParameter(int index, float ratio, float fixed, bool isRatio, float amplitude, float phase);

private:
    void calculateEnvelopeSegments();
    void calculateAmplutude();

    
    int domainResolution = 128;
    int envelopeSegments = 72;
    float op0Phase = 0.0f, op1Phase = 0.0f, op2Phase = 0.0f, op3Phase = 0.0f;
    std::vector<float> segmentAmplitude;
    std::array<std::array<juce::SmoothedValue<float>, 72>, 4> ampSmooth;
    juce::dsp::FastMathApproximations fastSin;
    struct operatorValues
    {
        float ratio, fixed, amplitude, phase;
        bool isRatio;
        float attack = 3000.0f, decay = 1000.0f, sustain = 1.0f, release = 5000.0f;
        float attackSegment, decaySegment, sustainSegment = 12, releaseSegment;
        std::array<float, 4> gain = { 0.0f, 0.0f, 0.0f, 0.0f };

        float generateAmplitude(float segmentIndex)
        {
            float amplitude = 0.0f;
            
            if (attackSegment > 0 && segmentIndex <= attackSegment) // attack portion
            {
                amplitude = (1.0 / attackSegment) * segmentIndex;
                
            } else if (segmentIndex > attackSegment && segmentIndex <= attackSegment + decaySegment)
            {
                int decayIndex = segmentIndex - attackSegment;
                float decayProgress = decayIndex / decaySegment;
                amplitude = 1.0f + decayProgress * (sustain - 1.0f);
                
            } else if (segmentIndex > attackSegment + decaySegment && segmentIndex <= attackSegment + decaySegment + sustainSegment) {
                amplitude = sustain;

            } else if (segmentIndex > attackSegment + decaySegment + sustainSegment && segmentIndex <= attackSegment + decaySegment + sustainSegment + releaseSegment)
                // release portion
            {
                int releaseIndex = segmentIndex - (attackSegment + decaySegment + sustainSegment);
                float releaseProgress = releaseIndex / releaseSegment;
                amplitude = sustain * (1.0f - releaseProgress);

            } else {
                amplitude = 0.0f;
            }
            return amplitude;
        }
    };
    
    std::array<float, 4> outputGain = { 1.0f, 0.0f, 0.0f, 0.0f };
    std::array<operatorValues, 4> op;
};
