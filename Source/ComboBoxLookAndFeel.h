/*
  ==============================================================================

    ComboBoxLookAndFeel.h
    Created: 22 Jul 2025 3:25:32pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class ComboBoxGraphics : public juce::LookAndFeel_V4
{
public:
    ComboBoxGraphics()
    {
        setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
        
    }
    
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox) override
    {
        auto bounds = juce::Rectangle<int>(width, height).toFloat();
        bounds.reduce(5, 5);
        juce::Path comboBoxPath, trianglePath, buttonPath;
        
        comboBoxPath.addRoundedRectangle(bounds, 5.0f);
        g.setColour(juce::Colour(40, 42, 41));
        g.fillPath(comboBoxPath);
        
        auto buttonBounds = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH);
        buttonPath.addRoundedRectangle(buttonBounds, 3.0f);
        
        
        g.setFont(12.0f);
        g.setColour(Colors::textColor);
        g.drawText(comboBox.getText(), bounds, juce::Justification::centred, false);
    }
    
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) override
    {
        juce::Path menuPath;
        menuPath.addRectangle(area);
        
        juce::Colour menuColor;
        menuColor = isHighlighted ? juce::Colour(40, 42, 41) : juce::Colour(40, 42, 41);
        g.setColour(menuColor);
        g.fillPath(menuPath);
        
        g.setColour(Colors::textColor);
        g.setFont(12.0f);
        g.drawText(text, area, juce::Justification::centred, false);
    }
    
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        juce::Path menuPath;
        menuPath.addRectangle(0, 0, width, height);
        g.setColour(juce::Colour(40, 42, 41));
        g.fillPath(menuPath);
    }
    
    void drawResizableFrame(juce::Graphics& g, int w, int h, const juce::BorderSize<int>& b) override {}
};

