/*
  ==============================================================================

    Graphics.h
    Created: 30 May 2025 2:07:31pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class OperatorControlGraphics : public juce::Component
{
public:
    void paint(juce::Graphics &g) override;
    void resized() override;
    
    void setParameters(bool isRatio, float ratio, float fixed, float modIndex);
    
    void mouseDown(const juce::MouseEvent &m) override;
    void mouseDrag(const juce::MouseEvent &m) override;
    void mouseUp(const juce::MouseEvent &m) override;
    
private:
    bool isRatio;
    float freq = 1.0f, amount = 1.0f; // testVariables
    
    float ratio, fixed, modIndex;
};
