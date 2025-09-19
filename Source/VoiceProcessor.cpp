/*
  ==============================================================================

    VoiceProcessor.cpp
    Created: 19 May 2025 2:49:25pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "VoiceProcessor.h"
#include <JuceHeader.h>

void SynthVoice::prepareToPlay(double sampleRate, float samplesPerBlock, int numChannels)
{
    this->sampleRate = sampleRate;
    for (int i = 0; i < 4; i++)
    {
        op[i].prepareToPlay(sampleRate, samplesPerBlock, numChannels);
    }
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    for (int i = 0; i < 4; i++)
    {
        op[i].startNote();
        op[i].setNoteNumber(midiNoteNumber);
        op[i].setVelocity(velocity);
        op[i].setPitchbend(currentPitchWheelPosition);//init pitchWheel position
        op[i].setPressure(1.0f);//init pressure value for no channel pressure
    }
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    for (int i = 0; i < 4; i++)
    {
        op[i].stopNote();
    }
    clearCurrentNote();
}

void SynthVoice::setEnvelope(int index, float attack, float decay, float sustain, float release, float globalAttack, float globalDecay, float globalSustain, float globalRelease)
{
    attackScaled = std::pow(2.0f, globalAttack / 100.0f) * attack;
    decayScaled = std::pow(2.0f, globalDecay / 100.0f) * decay;
    
    sustainScaled = juce::jlimit(0.0f, 1.0f, (globalSustain / 100.0f) * (sustain/100.0f));
    releaseScaled = std::pow(2.0f, globalRelease / 100.0f) * release;

    op[index].ampEnvelope.setEnvelopeParameters(attackScaled, decayScaled, sustainScaled, releaseScaled);
}

void SynthVoice::setFMParameters(int index, float ratio, float fixed, bool isFixed, float amplitude, float phase, float globalModIndex, float gainInDecibel)
{
    op[index].setOperator(ratio, fixed, isFixed, amplitude, phase, globalModIndex);
    
    this->gainInAmp = juce::Decibels::decibelsToGain(gainInDecibel);
}
    
void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    
    if (op[0].ampEnvelope.isActive())
    {
        for (int sample = startSample; sample < startSample + numSamples; ++sample) {
            op3 = op[3].processOperator(op0 * op3Gain[0],
                                        op1 * op3Gain[1],
                                        op2 * op3Gain[2],
                                        op3 * op3Gain[3]);
            
            op2 = op[2].processOperator(op0 * op2Gain[0],
                                        op1 * op2Gain[1],
                                        op2 * op2Gain[2],
                                        op3 * op2Gain[3]);
            
            op1 = op[1].processOperator(op0 * op1Gain[0],
                                        op1 * op1Gain[1],
                                        op2 * op1Gain[2],
                                        op3 * op1Gain[3]);
            
            op0 = op[0].processOperator(op0 * op0Gain[0],
                                        op1 * op0Gain[1],
                                        op2 * op0Gain[2],
                                        op3 * op0Gain[3]);

            output = op0 * outputGain[0] +
            op1 * outputGain[1] +
            op2 * outputGain[2] +
            op3 * outputGain[3];
            
            outputSample = output * 0.25f * gainInAmp;
            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            {
                outputBuffer.addSample(channel, sample, outputSample);
            }
        }
    }
}

void SynthVoice::setOperatorGain(int index, int gainIndex, int outputGainIndex)
{
    outputGain = toBinary4(outputGainIndex);

    switch(index){
        case 0:
            op0Gain = toBinary4(gainIndex);
            break;
        case 1:
            op1Gain = toBinary4(gainIndex);
            break;
        case 2:
            op2Gain = toBinary4(gainIndex);
            break;
        case 3:
            op3Gain = toBinary4(gainIndex);
            break;
    }
}

float SynthVoice::getOutputSample()
{
    return outputSample;
}

