/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UserInterface.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class FledgeAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::AudioProcessorParameter::Listener
{
public:
    FledgeAudioProcessorEditor (FledgeAudioProcessor&);
    ~FledgeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        for (int oper = 0; oper < 4; oper++)
        {
            juce::String attackID = "attack" + juce::String(oper);
            juce::String decayID = "decay" + juce::String(oper);
            juce::String sustainID = "sustain" + juce::String(oper);
            juce::String releaseID = "release" + juce::String(oper);

            float attack = audioProcessor.apvts.getRawParameterValue(attackID)->load();
            float decay = audioProcessor.apvts.getRawParameterValue(decayID)->load();
            float sustain = audioProcessor.apvts.getRawParameterValue(sustainID)->load();
            float release = audioProcessor.apvts.getRawParameterValue(releaseID)->load();
            
            waveformDisplay.setEnvelope(oper, attack, decay, sustain, release);
            
            juce::String ratioID = "ratio" + juce::String(oper);
            juce::String modIndexID = "modIndex" + juce::String(oper);
            float ratio = audioProcessor.apvts.getRawParameterValue(ratioID)->load();
            float modIndex = audioProcessor.apvts.getRawParameterValue(modIndexID)->load();

            waveformDisplay.setFMParameter(oper, ratio, 0.0f, true, modIndex);
        }
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}


private:
    PracticeDialGraphics practiceLookAndFeel;
    juce::Slider practiceSlider;
    
        
    std::array<std::unique_ptr<OperatorInterface>, 4>  opInterface;
    WaveformDisplayGraphics waveformDisplay;
    FledgeAudioProcessor& audioProcessor;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessorEditor)
};
