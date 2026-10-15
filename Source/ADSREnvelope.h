/*
  ==============================================================================

    ADSREnvelope.h
    Created: 9 Aug 2025 2:43:09pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

enum class EnvState { Idle, Attack, Decay, Sustain, Release };
class ADSREnvelope
{
public:
    
    void setSampleRate(double sampleRate)
    {
        this->sampleRate = sampleRate;
    }

    void setEnvelopeParameters(float attack, float decay, float sustain, float release)
    {
        attackInc  = 1.0f / juce::jmax(1.0f, (float) (attack * 0.001 * sampleRate));
        decayInc  = 1.0f / juce::jmax(1.0f, (float) (decay * 0.001 * sampleRate));
        this->sustain = sustain;
        releaseSamples = juce::jmax(1.0f, (float) (release * 0.001 * sampleRate));
    }
    
    bool isActive()
    {
        bool envelopeIsActive = (envelopeValue > 0.00001f || state != EnvState::Idle);
        return envelopeIsActive;
    }
    
    void noteOn()
    {
        state = EnvState::Attack;
    }
    void noteOff()
    {
        if (state == EnvState::Idle || state == EnvState::Release)
            return;

        // Recalculate the slope from the level at the moment of gate release
        releaseInc = envelopeValue / releaseSamples;
        state = EnvState::Release;
    }

    float getNextSample()
    {
        switch (state)
        {
            case EnvState::Idle:
                break;

            case EnvState::Attack:
                envelopeValue += attackInc;
                if (envelopeValue >= 1.0f)
                {
                    envelopeValue = 1.0f;
                    state = EnvState::Decay;
                }
                break;

            case EnvState::Decay:
                envelopeValue -= decayInc;
                if (envelopeValue <= sustain)
                {
                    envelopeValue = sustain;
                    state = EnvState::Sustain;
                    
                }
                break;

            case EnvState::Sustain:
                break;

            case EnvState::Release:
                envelopeValue -= releaseInc;
                if (envelopeValue <= 0.0f)
                {
                    envelopeValue = 0.0f;
                    state = EnvState::Idle;
                }
                break;
        }
        return envelopeValue;
        
    }
    
private:
    double sampleRate = 44100.0;
    float envelopeValue = 0.0f;
    float attackInc = 0.0f, decayInc = 0.0f, sustain = 0.5f, releaseInc = 0.0f;
    float releaseSamples = 1.0f;
    EnvState state = EnvState::Idle;
};
