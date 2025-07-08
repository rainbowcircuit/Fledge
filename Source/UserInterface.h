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
    
    void paint(juce::Graphics &g) override;
    void resized() override;
    void setLabel(juce::Label &l, juce::String labelText, float size);
    
    void setIndex(int index);
    
    void timerCallback() override;
    
private:
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


class AlgorithmInterface : public juce::Component
{
public:
    
    
private:
    
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
    
    /*
    ButtonGraphics saveLAF { 0 }, nextLAF { 1 }, prevLAF { 2 };
    ComboBoxGraphics comboBoxLAF;
    */
     
    juce::TextButton saveButton, nextButton, prevButton;
    juce::ComboBox presetComboBox;
    juce::Label rateLabel, rateValueLabel;
    
    std::unique_ptr<juce::FileChooser> fileChooser;

    PresetManager presetManager;
    FledgeAudioProcessor& audioProcessor;
};








class PracticeDialGraphics : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        g.fillAll(juce::Colour(255, 255, 255));
        
        for (int i = 0 ; i < 6; i++)
        {
            float lineIncr = (height/6) * sliderPosProportional * i;
            
            juce::Point<float> leftCoords = { (float)x, (float)y + lineIncr };
            juce::Point<float> rightCoords = { (float)x + width, (float)y + lineIncr };
            
            juce::Path linePath;
            linePath.startNewSubPath(leftCoords);
            linePath.lineTo(rightCoords);
            
            g.setColour(juce::Colour(155, 155, 155));
            
            if (slider.isMouseOverOrDragging())
            {
                g.setColour(juce::Colour(200, 200, 200));
            }
            
            g.strokePath(linePath, juce::PathStrokeType(1.0f));
        }
    }

private:
    
    
};


