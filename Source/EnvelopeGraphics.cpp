/*
  ==============================================================================

    EnvelopeGraphics.cpp
    Created: 8 Aug 2025 2:32:00pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "EnvelopeGraphics.h"

EnvelopeDisplayGraphics::EnvelopeDisplayGraphics(FledgeAudioProcessor &p, int index) : audioProcessor(p)
{
    this->index = index;
    startTimerHz(60);
}

void EnvelopeDisplayGraphics::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    fillControlPanel(g, bounds);
    
    float x = bounds.getX();
    float y = bounds.getY();
    auto envelopeBounds = bounds;
    envelopeBounds.reduce(5, 5);

    float width = envelopeBounds.getWidth() * 0.8f;
    float height = envelopeBounds.getHeight() * 0.8f;
    float widthMargin = envelopeBounds.getWidth() * 0.1f;
    float heightMargin = envelopeBounds.getHeight() * 0.1f;
    
    calculateSegment();

    // adjusted foreground
    drawSegment(g, x + widthMargin, y + heightMargin, width, height);
    
    for (int i = 0; i < 5; i++)
    {
        pointsGlobalAdjusted[i].drawHandles(g);
    }
}

void EnvelopeDisplayGraphics::resized()
{
    calculateSegment();
}

float EnvelopeDisplayGraphics::calculateScaledPercentage(float segment, float total)
{
    if (total == 0.0f)
        return 0.0f; // avoid divide-by-zero

    float proportion = (segment / total);
    float scale = 80.0f * 0.01f;

    return proportion * scale;
}

void EnvelopeDisplayGraphics::calculateSegment()
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth() * 0.8f;
    float height = bounds.getHeight() * 0.8f;
    float widthMargin = bounds.getWidth() * 0.1f;
    float heightMargin = bounds.getHeight() * 0.1f;

    pointsGlobalAdjusted[0].coords = { x + widthMargin,
        y + height + heightMargin }; // Bottom (0)
    
    pointsGlobalAdjusted[1].coords = { x + widthMargin + width * attackPct,
        y + heightMargin }; // Top (1.0)
    
    pointsGlobalAdjusted[2].coords = { x + widthMargin + width * (attackPct + decayPct),
        y + heightMargin + height * (1.0f - sustain) }; // Sustain level
    
    pointsGlobalAdjusted[3].coords = { x + widthMargin + width * (attackPct + decayPct + sustainPct),
        y + heightMargin + height * (1.0f - sustain) }; // Same sustain level
    
    pointsGlobalAdjusted[4].coords = { x + widthMargin + width * (attackPct + decayPct + sustainPct + releasePct),
        y + height + heightMargin };
    repaint();
}


void EnvelopeDisplayGraphics::setEnvelope(float attack, float decay, float sustain, float release, float attackAdj, float decayAdj, float sustainAdj, float releaseAdj)
{
    this->attack = attack;
    this->decay = decay;
    this->sustain = sustain / 100.0f;
    this->release = release;
    
    float attackAdjusted = attack * std::pow(2.0f, attackAdj / 100.0f);
    float decayAdjusted = decay * std::pow(2.0f, decayAdj / 100.0f);
    float releaseAdjusted = release * std::pow(2.0f, releaseAdj / 100.0f);
    sustainLevelAdjusted = (sustain / 100.0) * std::pow(2.0f, sustainAdj / 100.0f);
    sustainLevelAdjusted = juce::jlimit(0.0f, 1.0f, sustainLevelAdjusted);
    
    // Sustain always takes 25% of width
    sustainPct = 0.25f;
    
    // Calculate A+D+R proportions for remaining 75%
    float adrSum = attack + decay + release;
    float adrAdjSum = attackAdjusted * decayAdjusted * releaseAdjusted;

    if (adrSum > 0.0f) {
        attackPct  = (attack / adrSum) * 0.75f;
        decayPct   = (decay / adrSum) * 0.75f;
        releasePct = (release / adrSum) * 0.75f;
        
    } else {
        attackPct  = 0.25f;
        decayPct   = 0.25f;
        releasePct = 0.25f;
    }
    
    if (adrAdjSum > 0.0f){
        attackAdjPct = (attackAdjusted / adrAdjSum) * 0.75f;
        decayAdjPct = (decayAdjusted / adrAdjSum) * 0.75f;
        releaseAdjPct = (releaseAdjusted / adrAdjSum) * 0.75f;
        
    } else {
        attackAdjPct = 0.25f;
        decayAdjPct = 0.25f;
        releaseAdjPct = 0.25f;
    }
    calculateSegment();
}

void EnvelopeDisplayGraphics::drawSegment(juce::Graphics &g, float x, float y, float width, float height)
{
    auto points = pointsGlobalAdjusted;
    
    juce::Path envelopePath;
    envelopePath.startNewSubPath(points[0].coords);
    envelopePath.lineTo(points[1].coords);
    envelopePath.cubicTo(points[1].coords.x, points[2].coords.y,
                         points[1].coords.x + width * decayPct * 0.5f, points[2].coords.y,
                         points[2].coords.x, points[2].coords.y);
    envelopePath.lineTo(points[3].coords);
    envelopePath.cubicTo(points[3].coords.x, points[4].coords.y,
                         points[3].coords.x + width * releasePct * 0.5f, points[4].coords.y,
                         points[4].coords.x, points[4].coords.y);
    
    g.setColour(Colors::mainColors[index].withAlpha((float)0.15f));
    g.fillPath(envelopePath);
    
    g.setColour(Colors::mainColors[index]);
    juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.strokePath(envelopePath, strokeType);
    
    
}

void EnvelopeDisplayGraphics::timerCallback()
{
    auto mouse = getMouseXYRelative().toFloat();
    for (int i = 1; i < 5; i++) // ignore init segment
    {
        pointsGlobalAdjusted[i].isMouseOver = pointsGlobalAdjusted[i].isOver(mouse);
    }
}

void EnvelopeDisplayGraphics::mouseDown(const juce::MouseEvent &m)
{
    auto mouse = m.getPosition().toFloat();
    for (int i = 1; i < 5; i++) // ignore init segment
    {
        if (pointsGlobalAdjusted[i].isOver(mouse))
        {
            dragIndex = i;
            dragStartPoint = mouse;
            
            auto segmentParameterID = (getSegmentParamID(*dragIndex));
            initialParamValue = audioProcessor.params->apvts.getParameter(segmentParameterID)->getValue();
        }
    }
}

void EnvelopeDisplayGraphics::mouseDrag(const juce::MouseEvent &m)
{
    auto mousePoint = m.getPosition().toFloat();
    float sensitivity = 0.01f;

    if (dragIndex.has_value())
    {
        float deltaX = mousePoint.x - dragStartPoint.x;
        float newValueX = juce::jlimit(0.0f, 1.0f, initialParamValue + (deltaX * sensitivity));

        float deltaY = mousePoint.y - dragStartPoint.y;
        float newValueY = juce::jlimit(0.0f, 1.0f, initialParamValue + (-deltaY * sensitivity));

        if (*dragIndex == 1) {
            setEnvelopeParam(1, newValueX);

        } else if (*dragIndex == 2){
            setEnvelopeParam(2, newValueX);

        } else if (*dragIndex == 3){
            setEnvelopeParam(3, newValueY);

        } else if (*dragIndex == 4){
            setEnvelopeParam(4, newValueX);
        }
        repaint();
    }
}

void EnvelopeDisplayGraphics::mouseUp(const juce::MouseEvent &m)
{
    dragIndex.reset();
}

juce::String EnvelopeDisplayGraphics::getSegmentParamID(int segmentDragged)
{
    juce::String segmentParameterID;
    if (segmentDragged == 1){
        segmentParameterID = "attack" + juce::String(index);
        
    } else if (segmentDragged == 2){
        segmentParameterID = "decay" + juce::String(index);
        
    } else if (segmentDragged == 3){
        segmentParameterID = "sustain" + juce::String(index);
    
    } else if (segmentDragged == 4){
        segmentParameterID = "release" + juce::String(index);

    }

    return segmentParameterID;
}

void EnvelopeDisplayGraphics::setEnvelopeParam(int segmentDragged, float adjustAmount)
{
    auto segmentParameterID = getSegmentParamID(segmentDragged);
    auto paramRange = audioProcessor.params->apvts.getParameterRange(segmentParameterID);
    
    audioProcessor.params->apvts.getParameter(segmentParameterID)->setValueNotifyingHost(adjustAmount);
    
}
