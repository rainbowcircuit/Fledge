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

class EditableTextBoxSlider : public juce::Component , juce::AudioProcessorParameter::Listener, juce::AsyncUpdater, juce::Label::Listener
{
public:
    EditableTextBoxSlider(FledgeAudioProcessor& p, juce::String parameterID, juce::String parameterSuffix) : audioProcessor(p)
    {
        this->parameterID = parameterID;
        this->parameterSuffix = parameterSuffix;
        
        addAndMakeVisible(textBox);
        textBox.setEditable(false, false, false);
        textBox.setInterceptsMouseClicks(false, false);
        
        // initialize displayed value
        auto value = audioProcessor.apvts.getRawParameterValue(parameterID)->load();
        juce::String formattedValue = juce::String(value, 2) + parameterSuffix;
        textBox.setText(formattedValue, juce::dontSendNotification);
        textBox.addListener(this);
        
        // initialize parameter ranges
        juce::NormalisableRange range = audioProcessor.apvts.getParameterRange(parameterID);
        rangeStart = range.start;
        rangeEnd = range.end;
        
        // add listener
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->addListener(this);
        }
    }
    
    ~EditableTextBoxSlider()
    {
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->removeListener(this);
        }
    }
    
    void paint(juce::Graphics& g) override {}
    
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

        float value = juce::jlimit(0.0f, 1.0f, deltaY/100.0f); // clamp this
        textValueToParamValue(value);
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
    }
    
    void mouseDoubleClick(const juce::MouseEvent& m) override
    {
        textBox.setInterceptsMouseClicks(true, true);
        textBox.setEditable(true, true, false);
        textBox.showEditor();
        textBox.grabKeyboardFocus();
    }
    
    void labelTextChanged(juce::Label* l) override
    {
        auto value = l->getText().getFloatValue();
        float valueLimited = juce::jlimit(rangeStart, rangeEnd, value);
        
        l->setText(juce::String(valueLimited, 2), juce::dontSendNotification);
        textBox.setInterceptsMouseClicks(false, false);
        
        float normalized = (valueLimited - rangeStart) / (rangeEnd - rangeStart);
        textValueToParamValue(normalized);
        repaint();
    }

    void textValueToParamValue(float value)
    {
        value = juce::jlimit(0.0f, 1.0f, value);
        audioProcessor.apvts.getParameter(parameterID)->setValueNotifyingHost(value);
    }
        
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        newValueAtomic.store(newValue);
        parameterIndexAtomic.store(parameterIndex);
        triggerAsyncUpdate();
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    
    void handleAsyncUpdate() override
    {
        float newValue = newValueAtomic.load();
        int parameterIndex = parameterIndexAtomic.load();
        juce::String newParameterID;
        float scaledValue;
        
        if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[parameterIndex]))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                scaledValue = rangedParam->convertFrom0to1(newValue);
                newParameterID = param->paramID;
            }
        }
        
        if (newParameterID == parameterID)
        {
            juce::String formattedValue = juce::String(scaledValue, 2) + parameterSuffix;
            textBox.setText(formattedValue, juce::dontSendNotification);
        }
    }

    void setFontSize(float size)
    {
        textBox.setFont(juce::FontOptions(size, juce::Font::plain));
    }
    
private:
    float rangeStart, rangeEnd;
    std::atomic<float> newValueAtomic;
    std::atomic<int> parameterIndexAtomic;
    
    juce::Point<float> dragStartPoint;
    juce::Label textBox;
    juce::String parameterID, parameterSuffix = "";
    
    FledgeAudioProcessor& audioProcessor;
};


class ButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ButtonLookAndFeel(int graphicIndex)
    {
        this->graphicIndex = graphicIndex;
    }
    
    void drawButtonBackground (juce::Graphics &g, juce::Button &button, const juce::Colour &backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        float xPos = bounds.getX();
        float yPos = bounds.getY();
        float size = bounds.getHeight();
        float graphicWidth = bounds.getWidth();
        float graphicHeight = bounds.getHeight();

        if (graphicIndex == 0)
        {
            drawSaveButton(g, xPos, yPos, size, size);
        } else if (graphicIndex == 1)
        {
            drawArrowButton(g, xPos, yPos, size, size, false);
        } else if (graphicIndex == 2)
        {
            drawArrowButton(g, xPos, yPos, size, size, true);
        } else if (graphicIndex == 3)
        {
            
            drawBlockIcon(g, xPos + (graphicWidth/2) - (size/2), yPos, size, shouldDrawButtonAsDown);
        } else if (graphicIndex == 4)
        {
            drawWaveIcon(g, xPos + (graphicWidth/2) - (size/2), yPos, size, shouldDrawButtonAsDown);
        }
    }
    
    void drawSaveButton(juce::Graphics& g, float x, float y, float width, float height)
    {
        float graphicMargin = width * 0.3f;
        float graphicSize = width * 0.4f;
        x = x + graphicMargin;
        y = y + graphicMargin;

        juce::Point<float> topLeft { x, y };
        juce::Point<float> slopeStart { x + graphicSize * 0.75f, y };
        juce::Point<float> slopeEnd { x + graphicSize, y + graphicSize * 0.25f};
        juce::Point<float> botRight { x + graphicSize, y + graphicSize };
        juce::Point<float> botLeft { x, y + graphicSize };

        juce::Path bodyPath;
        bodyPath.startNewSubPath(topLeft);
        bodyPath.lineTo(slopeStart);
        bodyPath.lineTo(slopeEnd);
        bodyPath.lineTo(botRight);
        bodyPath.lineTo(botLeft);
        bodyPath.closeSubPath();
        bodyPath = bodyPath.createPathWithRoundedCorners(1.0f);
        g.setColour(juce::Colour(70, 204, 164));
        g.fillPath(bodyPath);
    }

    
    void drawArrowButton(juce::Graphics& g, float x, float y, float width, float height, bool isLeftArrow)
    {
        float graphicMargin = width * 0.4f;
        float graphicSize = width * 0.2f;
        x = x + graphicMargin;
        y = y + graphicMargin;

        // coordinates
        juce::Point<float> topLeft { x, y };
        juce::Point<float> botLeft { x, y + graphicSize };
        juce::Point<float> middleRight { x + graphicSize, y + graphicSize/2 };
        juce::Point<float> topRight { x + graphicSize, y };
        juce::Point<float> middleLeft { x, y + graphicSize/2 };
        juce::Point<float> botRight { x + graphicSize, y + graphicSize };

        // drawing
        juce::Path arrowPath;
        if (isLeftArrow){
            arrowPath.startNewSubPath(topLeft);
            arrowPath.lineTo(botLeft);
            arrowPath.lineTo(middleRight);
            arrowPath.closeSubPath();
        } else {
            arrowPath.startNewSubPath(topRight);
            arrowPath.lineTo(botRight);
            arrowPath.lineTo(middleLeft);
            arrowPath.closeSubPath();
        }

        arrowPath = arrowPath.createPathWithRoundedCorners(1.0f);
        g.setColour(juce::Colour(70, 204, 164));
        g.fillPath(arrowPath);
    }

    
    void drawBlockIcon(juce::Graphics &g, float x, float y, float size, bool buttonDown)
    {
        juce::Path blockPath, linePath;
        float offset = size * 0.175f;
        float blockSize = size * 0.225f;
        
        g.setColour(juce::Colour(70, 70, 70));
        g.fillAll();
        
        juce::Point<float> centerCoords = {x + size/2, y + size/2};
        juce::Point<float> topCoords = {centerCoords.x, centerCoords.y - offset};
        juce::Point<float> botLeftCoords = {centerCoords.x - offset, centerCoords.y + offset};
        juce::Point<float> botRightCoords = {centerCoords.x + offset, centerCoords.y + offset};

        linePath.startNewSubPath(topCoords.x, topCoords.y + blockSize/2);
        linePath.lineTo(botLeftCoords.x, botLeftCoords.y - blockSize/2);

        linePath.startNewSubPath(topCoords.x, topCoords.y + blockSize/2);
        linePath.lineTo(botRightCoords.x, botLeftCoords.y - blockSize/2);
        g.setColour(juce::Colour(70, 204, 164));
        g.strokePath(linePath, juce::PathStrokeType(1.0f));

        
        blockPath.addRoundedRectangle(topCoords.x - blockSize/2, topCoords.y - blockSize/2, blockSize, blockSize, 2.0f);
        blockPath.addRoundedRectangle(botLeftCoords.x - blockSize/2, botLeftCoords.y - blockSize/2, blockSize, blockSize, 2.0f);
        blockPath.addRoundedRectangle(botRightCoords.x - blockSize/2, botRightCoords.y - blockSize/2, blockSize, blockSize, 2.0f);

        g.setColour(juce::Colour(70, 204, 164)); // different color here
        g.fillPath(blockPath);
        g.setColour(juce::Colour(70, 204, 164));
        g.strokePath(blockPath, juce::PathStrokeType(1.0f));
    }
    
    void drawWaveIcon(juce::Graphics &g, float x, float y, float size, bool buttonDown)
    {
        g.setColour(juce::Colour(70, 70, 70));
        g.fillAll();

        juce::Path linePath;

        for (int i = 0; i < 3; i++)
        {
            float yIncr = (size/6) * (i + 3);
            float xIncr = (size/5) * (i + 1);
            float height = size * 0.25f;
            juce::Point<float> leftCoords = {x + size * 0.05f, y + yIncr };
            juce::Point<float> rightCoords = {x + size * 0.9f, y + yIncr };
            
            juce::Point<float> midCoords = {leftCoords.x + xIncr,
                y + yIncr - height };

            linePath.startNewSubPath(leftCoords);
            linePath.lineTo(midCoords.x - 2.0f, leftCoords.y); // maybe dont hard code margins
            linePath.lineTo(midCoords);
            linePath.lineTo(midCoords.x + 2.0f, rightCoords.y);
            linePath.lineTo(rightCoords);
            linePath = linePath.createPathWithRoundedCorners(4.0f);
            g.setColour(juce::Colour(70, 204, 164));
            g.strokePath(linePath, juce::PathStrokeType(1.0f));

        }
        
        
    }
    
private:
    int graphicIndex;
    
};
