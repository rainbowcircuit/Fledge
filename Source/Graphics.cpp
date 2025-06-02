/*
  ==============================================================================

    Graphics.cpp
    Created: 30 May 2025 2:07:31pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "Graphics.h"
/*
void OperatorControlGraphics::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    juce::Path graphicPath;
    
    float incr = (width * 0.8f)/48;
    graphicPath.startNewSubPath(x, y + height/4);
    
    for (int i = 0; i < 48; i++)
    {
        float sin = std::sin(i/((freq + 1.0f)/10));
        graphicPath.lineTo(incr * i + i, y + height/4 + (sin * (amount * 0.15f)));
    }
    graphicPath.lineTo(incr * 48 + 48, y + height/4 + height);

    
    graphicPath = graphicPath.createPathWithRoundedCorners(2);
    g.setColour(juce::Colour(200, 200, 200));
    g.strokePath(graphicPath, juce::PathStrokeType(2));
}

void OperatorControlGraphics::resized()
{
}

void OperatorControlGraphics::setParameters(bool isRatio, float ratio, float fixed, float modIndex)
{
    
    
}

void OperatorControlGraphics::mouseDown(const juce::MouseEvent &m)
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    auto mouse = m.getPosition();
    freq = mouse.getX();
    if (freq <=  x) {
        freq = x;
    } else if (freq >= x + width){
        freq = x + width;
    }
    
    amount = mouse.getY();
    if (amount <=  y) {
        amount = y;
    } else if (amount >= y + height){
        amount = y + height;
    }

    repaint();
}
void OperatorControlGraphics::mouseDrag(const juce::MouseEvent &m)
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    auto mouse = m.getPosition();
    freq = mouse.getX();
    if (freq <=  x) {
        freq = x;
    } else if (freq >= x + width){
        freq = x + width;
    }
    
    amount = mouse.getY();
    if (amount <=  y) {
        amount = y;
    } else if (amount >= y + height){
        amount = y + height;
    }

    repaint();
}

void OperatorControlGraphics::mouseUp(const juce::MouseEvent &m)
{
    
    
}
*/
