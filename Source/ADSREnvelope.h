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
        attackInc  = 1.0f / (attack * 0.001f * sampleRate);
        decayInc  = 1.0f / (decay * 0.001f * sampleRate);
        sustain  = sustain/100.0f;
        releaseInc  = 1.0f / (release * 0.001f * sampleRate);
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
    double sampleRate;
    bool gate;
    float envelopeValue = 0.0f;
    float attackInc = 0.0f, decayInc = 0.0f, sustain = 0.5f, releaseInc = 0.0f;
    EnvState state = EnvState::Idle;
};
