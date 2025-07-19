/*
  ==============================================================================

    UserInterface.h
    Created: 30 May 2025 11:28:23am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Graphics.h"
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "DialLookAndFeel.h"
#include "AlgorithmGraphics.h"
#include "Presets.h"

class MainInterface : public juce::Component
{
public:
    
    
private:
    juce::Slider modIndexSlider, attackSlider, decaySlider, sustainSlider, releaseSlider;
    
};


class OperatorInterface : public juce::Component, juce::Timer
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

    OperatorDisplayGraphics opGraphics;
    EnvelopeDisplayGraphics envGraphics;
    FledgeAudioProcessor& audioProcessor;
};


class AlgorithmSelectInterface : public juce::Component, public juce::Button::Listener
{
public:
    AlgorithmSelectInterface()
    {
        for(int i = 0; i < 8; i++)
        {
            addAndMakeVisible(algorithm[i]);
            algorithmGraphics[i].setIndex(i);
            algorithm[i].setLookAndFeel(&algorithmGraphics[i]);
            algorithm[i].addListener(this);

        }
    }
    
    ~AlgorithmSelectInterface()
    {
        for(int i = 0; i < 8; i++)
        {
            algorithm[i].setLookAndFeel(nullptr);
            algorithm[i].removeListener(this);

        }
    }

    void paint(juce::Graphics& g) override {}
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        
        float width = bounds.getWidth() * 0.8f;
        float height = bounds.getHeight() * 0.8f;
        float widthMargin = bounds.getWidth() * 0.1f;
        float heightMargin = bounds.getHeight() * 0.1f;

        float blockWidth = width * 0.2f;
        float blockHeight = height/2;

        for(int i = 0; i < 8; i++)
        {
            algorithm[i].setBounds(x + widthMargin + blockWidth * (i % 4),
                                   y + heightMargin + blockWidth * (i / 4),
                                   blockWidth,
                                   blockHeight);
        }
    }
    

    void buttonClicked(juce::Button* button) override
    {
        if (button == &algorithm[0]){

        } else if (button == &algorithm[1]) {

        } else if (button == &algorithm[2]) {
            
        } else if (button == &algorithm[3]) {
            
        } else if (button == &algorithm[4]) {
            
        } else if (button == &algorithm[5]) {
            
        } else if (button == &algorithm[6]) {
            
        } else if (button == &algorithm[7]) {
            
        }
    }
    
    
private:
    std::array<BlockDiagrams, 8> algorithmGraphics;
    std::array<juce::ToggleButton, 8> algorithm;
};


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
    
    juce::TextButton saveButton, nextButton, prevButton;
    juce::ComboBox presetComboBox;
    juce::Label rateLabel, rateValueLabel;
    
    std::unique_ptr<juce::FileChooser> fileChooser;

    PresetManager presetManager;
    FledgeAudioProcessor& audioProcessor;
};
