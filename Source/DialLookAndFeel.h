/*
  ==============================================================================

    DialLookAndFeel.h
    Created: 2 Jul 2025 7:31:43am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class DialLookAndFeel : public juce::LookAndFeel_V4
{
public:
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();
        float xPos = bounds.getX();
        float yPos = bounds.getY();
        float graphicWidth = bounds.getWidth();
        float graphicHeight = bounds.getHeight();

    //    drawRoundDial(g, xPos, yPos, size, sliderPosProportional);
        
        
        displayText(g, xPos, yPos, graphicWidth, graphicHeight, juce::String(slider.getValue()));
    }
    
    void displayText(juce::Graphics &g, float x, float y, float width, float height, juce::String value)
    {
        g.setFont(juce::FontOptions(40.0f, juce::Font::plain));
        g.setColour(juce::Colour(255, 255, 255));
        g.drawText(value, x, y, width, height, juce::Justification::centredLeft);
    }

    void drawRoundDial(juce::Graphics &g, float x, float y, float size, float position)
    {
        //==============================================================================

        float pi = juce::MathConstants<float>::pi;
        float dialStart = 1.25f * pi;
        float dialEnd = 2.75f * pi;
        float sliderPositionScaled = 2.0f + (1.0f - position);
        float dialPositionInRadians = dialStart + sliderPositionScaled * (dialEnd - dialStart);
        

        juce::Path dialBodyPath, dialDotPath, dialOutlinePath, dialSelectPath, tensionLeftPath, tensionRightPath;
        
        float dialOutlineRadius = (size * 0.8f)/2;
        float dialBodyRadius = (size * 0.7f)/2;
        float dialDotRadius = (size * 0.5f)/2;

        dialOutlinePath.addCentredArc(x + size/2, x + size/2,
                                      dialOutlineRadius, dialOutlineRadius,
                                      0.0f, dialStart, dialEnd, true);
        g.setColour(juce::Colour(200, 200, 200)); //

        juce::PathStrokeType strokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(dialOutlinePath, juce::PathStrokeType(strokeType));

        //==============================================================================
        // dial body
        
        dialBodyPath.addCentredArc(x + size/2, y + size/2,
                                   dialBodyRadius, dialBodyRadius,
                                   0.0f, 0.0f, 6.28f, true);
        g.setColour(juce::Colour(200, 200, 200)); //
        g.fillPath(dialBodyPath);
        
        //==============================================================================
        // dial dot
        
        juce::Point<float> outlineCoords = {x + size/2 + std::sin(dialPositionInRadians) * dialDotRadius,
            x + size/2 + std::cos(dialPositionInRadians) * dialDotRadius};

        dialDotPath.addCentredArc(outlineCoords.x, outlineCoords.y,
                                  1.5, 1.5, 0.0f, 0.0f, pi * 2, true);
        
        g.setColour(juce::Colour(20, 20, 20)); //
        g.fillPath(dialDotPath);
    }
    
private:

};


class EditableTextBoxSlider : public juce::Component //, juce::AudioProcessorListener
{
public:
    EditableTextBoxSlider(FledgeAudioProcessor& p, juce::String parameterID) : audioProcessor(p)
    {
        this->parameterID = parameterID;
        
        addAndMakeVisible(textBox);
        textBox.setEditable(false, true, true);
        textBox.setText("default text", juce::dontSendNotification);
        textBox.setInterceptsMouseClicks(false, false); // figure this one out

    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(100, 100, 100));
        // this  makes the box uneditable so do something else
    }
    
    void resized() override {
        auto bounds = getLocalBounds();
        textBox.setBounds(bounds);
        
    }
    
    void mouseDown(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
    }

    void mouseDrag(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        float deltaY = std::abs(mousePoint.y - dragStartPoint.y);
        
        float value = deltaY/100.0f; // clamp this
        textValueToParamValue(value);
        
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
    }

    void textValueToParamValue(float value)
    {
        if (value <= 0.0f) { value = 0.0f; }
        else if (value >= 1.0f) { value = 1.0f; }
        
        audioProcessor.apvts.getParameter(parameterID)->setValueNotifyingHost(value);
    }
    
    void paramValueToTextValue()
    {
        auto value = audioProcessor.apvts.getRawParameterValue(parameterID)->load();
        juce::String formattedValue = juce::String(value) + parameterSuffix;
        textBox.setText(formattedValue, juce::dontSendNotification);
    }
    
    void setSuffix(juce::String suffix)
    {
        parameterSuffix = suffix;
    }
    
    void setFontSize(float size)
    {
        textBox.setFont(juce::FontOptions(size, juce::Font::plain));
    }
    
private:
    juce::Point<float> dragStartPoint;
    juce::Label textBox;
    juce::String parameterID, parameterSuffix = "";
    
    FledgeAudioProcessor& audioProcessor;
};
