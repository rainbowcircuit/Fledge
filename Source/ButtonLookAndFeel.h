/*
  ==============================================================================

    ButtonLookAndFeel.h
    Created: 19 Jul 2025 3:22:03pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

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
            drawWaveIcon(g, xPos + (graphicWidth/2) - (size/2), yPos, size, shouldDrawButtonAsDown);

        } else if (graphicIndex == 4)
        {
            drawBlockIcon(g, xPos + (graphicWidth/2) - (size/2), yPos, size, shouldDrawButtonAsDown);

        } else if (graphicIndex == 5)
        {
            drawMacroIcon(g, xPos + (graphicWidth/2) - (size/2), yPos, size, shouldDrawButtonAsDown);
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
    
    void drawMacroIcon(juce::Graphics &g, float x, float y, float size, bool buttonDown)
    {
        g.setColour(juce::Colour(70, 70, 70));
        g.fillAll();

        juce::Path outlinePath, bodyPath, dotPath;
        juce::Point<float> centerCoords = { x + size/2, y + size/2 };
        float pi = juce::MathConstants<float>::pi;
        float outlineRadius = size * 0.3f;
        float bodyRadius = size * 0.25f;
        float dotRadius = size * 0.125f;

        outlinePath.addCentredArc(centerCoords.x, centerCoords.y, outlineRadius, outlineRadius, 0.0f, 1.25f * pi, 2.75f * pi, true);
        g.setColour(juce::Colour(70, 204, 164));
        g.strokePath(outlinePath, juce::PathStrokeType(1.0f));

        bodyPath.addCentredArc(centerCoords.x, centerCoords.y, bodyRadius, bodyRadius, 0.0f, 0.0f, 6.28f, true);
        g.setColour(juce::Colour(70, 204, 164));
        g.fillPath(bodyPath);
        
        dotPath.addCentredArc(centerCoords.x + dotRadius, centerCoords.y - dotRadius, 1.5f, 1.5f, 0.0f, 0.0f, 6.28f, true);
        g.setColour(juce::Colour(50, 50, 50));
        g.fillPath(dotPath);

    }

    
private:
    int graphicIndex;
    
};
