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

class MainInterface : public juce::Component
{
public:
    
    
private:
    juce::Slider modIndexSlider, attackSlider, decaySlider, sustainSlider, releaseSlider;
    
};


class OperatorInterface : public juce::Component
{
public:
    OperatorInterface(FledgeAudioProcessor& p, int index);
    
    void paint(juce::Graphics &g) override;
    void resized() override;
    void setSlider(juce::Slider &s, juce::Label &l, juce::String labelText);
    
    void setIndex(int index);
    
private:
    int index;
    
    juce::Label ratioLabel, fixedLabel, modIndexLabel, attackLabel, decayLabel, sustainLabel, releaseLabel;
    
    juce::Slider ratioSlider, fixedSlider, modIndexSlider, attackSlider, decaySlider, sustainSlider, releaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment, fixedAttachment, modIndexAttachment, attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;

    FledgeAudioProcessor& audioProcessor;
};


class AlgorithmInterface : public juce::Component
{
public:
    
    
private:
    
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
