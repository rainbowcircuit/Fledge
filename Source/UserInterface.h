/*
  ==============================================================================

    UserInterface.h
    Created: 30 May 2025 11:28:23am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PresetManager.h"
#include "Graphics.h"
#include "AlgorithmGraphics.h"
#include "DialLookAndFeel.h"
#include "ButtonLookAndFeel.h"
#include "LookAndFeel.h"
#include "GraphicsUtility.h"

class MacroControlsInterface : public juce::Component
{
public:
    MacroControlsInterface(FledgeAudioProcessor& p);
    void paint(juce::Graphics& g) override {}
    void resized() override;
    
    void setSliderAndLabel(juce::Slider &s, juce::Label &l, DialLookAndFeel &lookAndFeel, juce::String labelText, juce::String suffix)
    {
        addAndMakeVisible(l);
        l.setFont(juce::FontOptions(12.0f));
        l.setText(labelText, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, Colors::textColor);
        
        addAndMakeVisible(s);
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
        s.setLookAndFeel(&lookAndFeel);
        s.setTextValueSuffix(suffix);
    }
    
private:
    // look and feel
    DialLookAndFeel dialLAF;
    
    juce::Slider globalFreqSlider,
    globalModIndexSlider,
    globalAttackSlider,
    globalDecaySlider,
    globalSustainSlider,
    globalReleaseSlider;
    
    juce::Label globalFreqLabel,
    globalModIndexLabel,
    globalAttackLabel,
    globalDecayLabel,
    globalSustainLabel,
    globalReleaseLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> globalFreqAttachment,
    globalModIndexAttachment,
    globalAttackAttachment,
    globalDecayAttachment,
    globalSustainAttachment,
    globalReleaseAttachment;
    
    FledgeAudioProcessor& audioProcessor;
};
