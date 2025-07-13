/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UserInterface.h"
#include "AlgorithmGraphics.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class FledgeAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::AudioProcessorParameter::Listener, juce::Button::Listener
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
            juce::String amplitudeID = "amplitude" + juce::String(oper);
            float ratio = audioProcessor.apvts.getRawParameterValue(ratioID)->load();
            float modIndex = audioProcessor.apvts.getRawParameterValue(amplitudeID)->load();

            waveformDisplay.setFMParameter(oper, ratio, 0.0f, true, modIndex);
        }
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    void buttonClicked(juce::Button* buttonClicked) override
    {
        if (buttonClicked == &showWaveformButton)
        {
            waveformDisplay.setVisible(true);
            algorithmGraphics.setVisible(false);
            algorithmSelector.setVisible(false);

        } else if (buttonClicked == &showAlgorithmButton) {
            waveformDisplay.setVisible(false);
            algorithmGraphics.setVisible(true);
            algorithmSelector.setVisible(true);
        }
    }


private:
 //   TextBoxSlider laf;
    PracticeDialGraphics practiceLookAndFeel;
    juce::Slider practiceSlider;
        
    std::array<std::unique_ptr<OperatorInterface>, 4>  opInterface;
    std::unique_ptr<PresetInterface>  presetInterface;

    juce::TextButton showWaveformButton, showAlgorithmButton;
    WaveformDisplayGraphics waveformDisplay;
    AlgorithmGraphics algorithmGraphics;
    AlgorithmSelectInterface algorithmSelector;
    FledgeAudioProcessor& audioProcessor;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessorEditor)
};
