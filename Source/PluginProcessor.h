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
    
    void initializeParameters()
    {
        globalAttack = apvts.getRawParameterValue("globalAttack")->load();
        globalDecay = apvts.getRawParameterValue("globalDecay")->load();
        globalSustain = apvts.getRawParameterValue("globalSustain")->load();
        globalRelease = apvts.getRawParameterValue("globalRelease")->load();
        
        outputRouting = apvts.getRawParameterValue("outputRouting")->load();
        
        for (int oper = 0; oper < 4; oper++){
            juce::String attackID = "attack" + juce::String(oper);
            juce::String decayID = "decay" + juce::String(oper);
            juce::String sustainID = "sustain" + juce::String(oper);
            juce::String releaseID = "release" + juce::String(oper);
            
            attack[oper] = apvts.getRawParameterValue(attackID)->load();
            decay[oper] = apvts.getRawParameterValue(decayID)->load();
            sustain[oper] = apvts.getRawParameterValue(sustainID)->load();
            release[oper] = apvts.getRawParameterValue(releaseID)->load();
            
            juce::String ratioID = "ratio" + juce::String(oper);
            juce::String fixedID = "fixed" + juce::String(oper);
            juce::String amplitudeID = "amplitude" + juce::String(oper);
            juce::String phaseID = "phase" + juce::String(oper);
            
            juce::String operatorRoutingID = "operator" + juce::String(oper) + "Routing";
            
            ratio[oper] = apvts.getRawParameterValue(ratioID)->load();
            fixed[oper] = apvts.getRawParameterValue(fixedID)->load();
            amplitude[oper] = apvts.getRawParameterValue(amplitudeID)->load();
            phase[oper] = apvts.getRawParameterValue(phaseID)->load();
            routing[oper] = apvts.getRawParameterValue(operatorRoutingID)->load();
        }
    }
    
    
    
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        
        juce::String newParameterID;
        float scaledValue = 0.0f;

        if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(this->getParameters()[parameterIndex]))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                scaledValue = rangedParam->convertFrom0to1(newValue);
                newParameterID = param->paramID;
            }
        }
        globalAttack = juce::String("globalAttack") == newParameterID ? scaledValue : globalAttack;
        globalDecay = juce::String("globalDecay") == newParameterID ? scaledValue : globalDecay;
        globalSustain = juce::String("globalSustain") == newParameterID ? scaledValue : globalSustain;
        globalRelease = juce::String("globalRelease") == newParameterID ? scaledValue : globalRelease;
        
        for (int oper = 0; oper < 4; oper++)
        {
            juce::String incr = juce::String(oper);
            
            attack[oper] = juce::String("attack") + incr == newParameterID ? scaledValue : attack[oper];
            decay[oper] = juce::String("decay") + incr == newParameterID ? scaledValue : decay[oper];
            sustain[oper] = juce::String("sustain") + incr == newParameterID ? scaledValue : sustain[oper];
            release[oper] = juce::String("release") + incr == newParameterID ? scaledValue : release[oper];

            ratio[oper] = juce::String("ratio") + incr == newParameterID ? scaledValue : ratio[oper];
            fixed[oper] = juce::String("fixed") + incr == newParameterID ? scaledValue : fixed[oper];
            amplitude[oper] = juce::String("amplitude") + incr == newParameterID ? scaledValue : amplitude[oper];
            phase[oper] = juce::String("phase") + incr == newParameterID ? scaledValue : phase[oper];
            juce::String routingID = juce::String("operator") + incr + juce::String("Routing");
            routing[oper] = routingID == newParameterID ? scaledValue : routing[oper];
        }
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}

    
    
private:
    float globalAttack, globalDecay, globalSustain, globalRelease, outputRouting;
    bool isFixed;
    std::array<float, 4> attack, decay, sustain, release, ratio, fixed, amplitude, phase, routing;
    
    
    float outputLevel;
    std::atomic<float> levelAtomic;
    
    juce::Synthesiser synth;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessor)
};
