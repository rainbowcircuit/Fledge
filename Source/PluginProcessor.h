/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VoiceProcessor.h"

//==============================================================================
/**
*/
class FledgeAudioProcessor  : public juce::AudioProcessor, juce::AudioProcessorParameter::Listener
{
public:
    //==============================================================================
    FledgeAudioProcessor();
    ~FledgeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts {
        *this, nullptr, "Parameters", createParameterLayout()
    };
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        
        if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(this->getParameters()[parameterIndex]))
        {
            auto parameterID = param->getParameterID();
            // do something with this
        }

    
        /*
        float globalAttack = apvts.getRawParameterValue("globalAttack")->load();
        float globalDecay = apvts.getRawParameterValue("globalDecay")->load();
        float globalSustain = apvts.getRawParameterValue("globalSustain")->load();
        float globalRelease = apvts.getRawParameterValue("globalRelease")->load();
        
        for (int oper = 0; oper < 4; oper++){
            juce::String attackID = "attack" + juce::String(oper);
            juce::String decayID = "decay" + juce::String(oper);
            juce::String sustainID = "sustain" + juce::String(oper);
            juce::String releaseID = "release" + juce::String(oper);

            float attack = apvts.getRawParameterValue(attackID)->load();
            float decay = apvts.getRawParameterValue(decayID)->load();
            float sustain = apvts.getRawParameterValue(sustainID)->load();
            float release = apvts.getRawParameterValue(releaseID)->load();

            juce::String ratioID = "ratio" + juce::String(oper);
            juce::String fixedID = "fixed" + juce::String(oper);
            juce::String modIndexID = "amplitude" + juce::String(oper);

            float ratio = apvts.getRawParameterValue(ratioID)->load();
            float fixed = apvts.getRawParameterValue(fixedID)->load();
            float modIndex = apvts.getRawParameterValue(modIndexID)->load();

            for (int v = 0; v < synth.getNumVoices(); v++)
            {
                if(auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(v)))
                {
                    voice->setEnvelope(oper, attack, decay, sustain/100.0f, release,
                                       globalAttack, globalDecay, globalSustain, globalRelease);
                    voice->setFMParameters(oper, ratio, fixed, false, modIndex);
                }
            }
        }
         */
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}

    
    
private:
    float outputLevel;
    std::atomic<float> levelAtomic;
    
    juce::Synthesiser synth;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessor)
};
