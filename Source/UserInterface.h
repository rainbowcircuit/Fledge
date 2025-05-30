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

class MainInterface : public juce::Component
{
public:
    
    
private:
    juce::Slider modIndexSlider, attackSlider, decaySlider, sustainSlider, releaseSlider;
    
};


class OperatorInterface : public juce::Component
{
public:
    OperatorInterface();
    void paint(juce::Graphics &g) override;
    void resized() override;
    void setSlider(juce::Slider &s, juce::Label &l, juce::String labelText);
    
    void setIndex(int index);
    
private:
    int index;
    
    juce::Label ratioLabel, fixedLabel, modIndexLabel, attackLabel, decayLabel, sustainLabel, releaseLabel;
    
    juce::Slider ratioSlider, fixedSlider, modIndexSlider, attackSlider, decaySlider, sustainSlider, releaseSlider;
    
    OperatorControlGraphics operGraphic;
};


class AlgorithmInterface : public juce::Component
{
public:
    
    
private:
    
};
