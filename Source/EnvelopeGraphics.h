/*
  ==============================================================================

    EnvelopeGraphics.h
    Created: 8 Aug 2025 2:32:00pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GraphicsUtility.h"
#include "LookAndFeel.h"

class EnvelopeDisplayGraphics : public juce::Component, juce::Timer, GraphicsHelper
{
public:
    EnvelopeDisplayGraphics(FledgeAudioProcessor &p, int index);
    void paint(juce::Graphics &g) override;
    void resized() override;
    //==============================================================================

    float calculateScaledPercentage(float segment, float total);
    void calculateSegment();
    void setEnvelope(float attack, float decay, float sustain, float release, float attackAdj, float decayAdj, float sustainAdj, float releaseAdj);
    void drawSegment(juce::Graphics &g, float x, float y, float width, float height);
    void timerCallback() override;
    //==============================================================================

    void mouseDown(const juce::MouseEvent &m) override;
    void mouseDrag(const juce::MouseEvent &m) override;
    void mouseUp(const juce::MouseEvent &m) override;
    //==============================================================================

    juce::String getSegmentParamID(int segmentDragged);
    void setEnvelopeParam(int segmentDragged, float adjustAmount);
    
private:
    int index;
    float attackPct, decayPct, releasePct, sustainPct;
    float attack, decay, sustain, release;
    float attackAdjPct, decayAdjPct, releaseAdjPct;
    float sustainLevelAdjusted;
    
    juce::Point<float> dragStartPoint;
    float initialParamValue;
    
    struct Handle
    {
        juce::Point<float> coords;
        bool isMouseOver = false;
        bool xyAdjust;
        
        bool isOver(juce::Point<float>& m)
        {
            juce::Rectangle point(coords.x - 6.0f, coords.y - 6.0f, 12.0f, 12.0f);
            return (point.contains(m));
        }
        
        void drawHandles(juce::Graphics &g)
        {
            juce::Path handlePath;
            float size = isMouseOver ? 8.0f : 6.0f;
            juce::Colour color = isMouseOver ? juce::Colour(200, 200, 200) : juce::Colour(150, 150, 150);

            handlePath.addRoundedRectangle(coords.x - size/2, coords.y - size/2, size, size, size/4);
            g.setColour(color);
            g.strokePath(handlePath, juce::PathStrokeType(1.0f));
        }
    };
    
    std::array<Handle, 5> pointsNoAdjusted;
    std::array<Handle, 5> pointsGlobalAdjusted;

    std::optional<int> dragIndex;
    FledgeAudioProcessor& audioProcessor;
};
