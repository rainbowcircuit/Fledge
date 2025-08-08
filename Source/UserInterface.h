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
#include "Presets.h"
#include "Graphics.h"
#include "AlgorithmGraphics.h"
#include "DialLookAndFeel.h"
#include "ButtonLookAndFeel.h"
#include "ComboBoxLookAndFeel.h"
#include "LookAndFeel.h"
#include "GraphicsUtility.h"

class PresetInterface : public juce::Component, juce::ComboBox::Listener, juce::Button::Listener
{
public:
    PresetInterface(FledgeAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~PresetInterface();
    
    void paint(juce::Graphics& g) override {}
    
    void resized() override;
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* buttonClicked) override;
    void loadPresetList();
    
private:
    
    ButtonLookAndFeel saveLAF { 0 }, prevLAF{ 1 }, nextLAF { 2 };
    ComboBoxGraphics presetComboBoxLAF;
    juce::TextButton saveButton, nextButton, prevButton;
    juce::ComboBox presetComboBox;
    juce::Label rateLabel, rateValueLabel;
    
    std::unique_ptr<juce::FileChooser> fileChooser;

    PresetManager presetManager;
    FledgeAudioProcessor& audioProcessor;
};

class OperatorInterface : public juce::Component, juce::Timer, GraphicsHelper
{
public:
    OperatorInterface(FledgeAudioProcessor& p, int index);
    void setIndex(int index);

    void paint(juce::Graphics &g) override;
    void resized() override;
    
private:
    void setLabel(juce::Label &l, juce::String labelText, float size);
    void timerCallback() override;

    int index;
    
    juce::Label ratioLabel,
    fixedLabel,
    amplitudeLabel,
    phaseLabel,
    attackLabel,
    decayLabel,
    sustainLabel,
    releaseLabel;
    
    std::unique_ptr<EditableTextBoxSlider> ratioSlider,
    fixedSlider,
    amplitudeSlider,
    phaseSlider,
    attackSlider,
    decaySlider,
    sustainSlider,
    releaseSlider;

    std::unique_ptr<OperatorDisplayGraphics> opGraphics;
    std::unique_ptr<EnvelopeDisplayGraphics> envGraphics;
    FledgeAudioProcessor& audioProcessor;
};

class AlgorithmSelectInterface : public juce::Component, public juce::Button::Listener
{
public:
    AlgorithmSelectInterface(FledgeAudioProcessor& p, AlgorithmGraphics& a);
    ~AlgorithmSelectInterface();
    void paint(juce::Graphics& g) override {}
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void setOperatorParam(int index, int gainIndex);
    void setOutputParam(int gainIndex);
    void setPresetParam(int index);
    
private:
    std::array<BlockDiagrams, 8> algorithmGraphics;
    std::array<juce::ToggleButton, 8> algorithm;
    
    AlgorithmGraphics& algoGraphics;
    FledgeAudioProcessor& audioProcessor;
};

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
