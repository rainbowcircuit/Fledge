/*
  ==============================================================================

    GraphicsUtility.h
    Created: 8 Aug 2025 10:46:28am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class GraphicsHelper
{
public:
    void fillBackgroundPanel(juce::Graphics &g, juce::Rectangle<float> bounds)
    {
        bounds.reduce(5, 5);
        
        juce::Path boundsPath;
        boundsPath.addRoundedRectangle(bounds, 5, 5);
        g.setColour(juce::Colour(40, 42, 41));
        g.fillPath(boundsPath);
        juce::ColourGradient highlight = {juce::Colour(255, 255, 255).withAlpha((float)0.1f),
            bounds.getX(),
            bounds.getCentreY(),
            juce::Colour(255, 255, 255).withAlpha((float)0.025f),
            bounds.getX() + bounds.getWidth(),
            bounds.getCentreY(), false};
     //   g.setGradientFill(highlight);
     //   g.strokePath(boundsPath, juce::PathStrokeType(1.25f));
    }
    
    void fillControlPanel(juce::Graphics &g, juce::Rectangle<float> bounds)
    {
        bounds.reduce(5, 5);
        juce::Path bgFill;
        bgFill.addRoundedRectangle(bounds, 5.0f);
        g.setColour(Colors::controlPanelColor);
        g.fillPath(bgFill);
        g.setColour(Colors::controlPanelShadowColor);
        g.strokePath(bgFill, juce::PathStrokeType(1.0f));
    }
};
