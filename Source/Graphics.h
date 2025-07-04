/*
  ==============================================================================

    Graphics.h
    Created: 30 May 2025 2:07:31pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <cmath>
#include "LookAndFeel.h"

class OperatorDisplayGraphics : public juce::Component
{
public:
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        juce::Path bgFill;
        bgFill.addRoundedRectangle(bounds, 5.0f);
        g.setColour(juce::Colour(36, 35, 34));
        g.fillPath(bgFill);

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();

        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        juce::Path bgWaveform = waveformPath(g, x + width * 0.05f, y + width * 0.2f, width * 0.9f, height * 0.75f, 1.0f, 1.0f);
        g.setColour(juce::Colour(200, 200, 200));
        g.strokePath(bgWaveform, strokeType);
        
        juce::Path fgWaveform = waveformPath(g, x + width * 0.05f, y + width * 0.2f, width * 0.9f, height * 0.75f, ratio, modIndex/10.0f);
        g.setColour(juce::Colour(255, 255, 255));
        g.strokePath(fgWaveform, strokeType);

    }
    
    juce::Path waveformPath(juce::Graphics &g, float x, float y, float width, float height, float freq, float amp)
    {
        juce::Path graphicPath;
        graphicPath.startNewSubPath(x, y + height/2);
        int domainResolution = 128;
        float widthIncrement = width/domainResolution;
        for (int i = 0; i < domainResolution; i++)
        {
            
            float sin = 1.0f * std::sin((i/40.7f) * freq * 2.0f);
            graphicPath.lineTo(x + widthIncrement * i, (y + height/2) + (height * sin/2) * amp);
        }
    
        graphicPath = graphicPath.createPathWithRoundedCorners(4.0f);
        return graphicPath;
    }
    
    void resized() override
    {
    }
    
    void setRatioAndAmplitude(float ratio, float fixed, float modIndex, bool isRatio)
    {
        this->ratio = ratio;
        this->fixed = fixed;
        this->modIndex = modIndex;
        repaint();
    }
    
private:
    float ratio, fixed, modIndex;
};



class EnvelopeDisplayGraphics : public juce::Component
{
public:
    EnvelopeDisplayGraphics()
    {
        
    }
    
    
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        juce::Path bgFill;
        bgFill.addRoundedRectangle(bounds, 5.0f);
        g.setColour(juce::Colour(36, 35, 34));
        g.fillPath(bgFill);

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() * 0.9f;
        float height = bounds.getHeight() * 0.9f;
        float widthMargin = bounds.getWidth() * 0.05f;
        float heightMargin = bounds.getHeight() * 0.05f;
        
        drawSegment(g, x + widthMargin, y + heightMargin, width, height);
        
        for (int i = 0; i < 8; i++)
        {
            if (i != 5)
            handles[i].drawHandles(g);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() * 0.9f;
        float height = bounds.getHeight() * 0.9f;

        handles[0].coords = { x, y + height }; // initial
        handles[1].coords = { x + width * 0.1f, y + height * 0.5f }; // attack
        handles[2].coords = { x + width * 0.2f, y };
        handles[3].coords = { x + width * 0.3f, y + height * 0.25f }; // decay curve
        handles[4].coords = { x + width * 0.4f, y + height * 0.5f }; // sustain start
        handles[5].coords = { x + width * 0.8f, y + height * 0.5f }; // // sustain end
        handles[6].coords = { x + width * 0.9f, y + height * 0.75f }; // // sustain end
        handles[7].coords = { x + width, y + height };
        
        handles[0].yAdjustOnly = true;
        handles[1].yAdjustOnly = true;
        handles[2].yAdjustOnly = false;
        handles[3].yAdjustOnly = true;
        handles[4].yAdjustOnly = false;
        handles[5].yAdjustOnly = false;
        handles[6].yAdjustOnly = true;
        handles[7].yAdjustOnly = false;
    }
    
    void drawSegment(juce::Graphics &g, float x, float y, float width, float height)
    {
        juce::Path envelopePath;
        envelopePath.startNewSubPath(handles[0].coords);
        envelopePath.cubicTo(handles[1].coords, handles[1].coords, handles[2].coords);
        envelopePath.cubicTo(handles[2].coords, handles[2].coords, handles[3].coords);
        envelopePath.lineTo(handles[4].coords);
        envelopePath.lineTo(handles[5].coords.x, handles[4].coords.y);
        envelopePath.cubicTo(handles[6].coords, handles[6].coords, handles[7].coords);
        g.setColour(juce::Colour(255, 255, 255));
        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(envelopePath, strokeType);
    }

    void mouseDown(const juce::MouseEvent &m) override
    {
        auto mouse = m.getPosition().toFloat();
        for (int i = 0; i < 8; i++)
        {
            if (handles[i].isOver(mouse))
            {
                dragIndex = i;
            }
        }
    }
    
    void mouseDrag(const juce::MouseEvent &m) override
    {
        auto mouse = m.getPosition().toFloat();
        if (dragIndex.has_value())
        {
            if (!handles[*dragIndex].yAdjustOnly)
            {
                handles[*dragIndex].coords = { mouse.x, mouse.y };
            } else {
                handles[*dragIndex].coords.y = mouse.y;
            } 
            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent &m) override
    {
        dragIndex.reset();
    }
    
private:
    float attack, decay, sustain, release;

    struct Handle
    {
        juce::Point<float> coords;
        bool isMouseOver;
        bool yAdjustOnly;
        
        bool isOver(juce::Point<float>& m)
        {
            juce::Rectangle handle(coords.x - 5.0f, coords.y - 5.0f, 10.0f, 10.0f);
            return (handle.contains(m));
        }
        
        void drawHandles(juce::Graphics &g)
        {
            juce::Path handlePath;
            handlePath.addRoundedRectangle(coords.x - 3.0f, coords.y - 3.0f, 6.0f, 6.0f, 1.5f);
            g.setColour(juce::Colour(200, 200, 200));  // change via hover
            g.strokePath(handlePath, juce::PathStrokeType(1.0f));
        }
    };
    
    std::array<Handle, 8> handles;
    // initial, attack, peak, decay, sustain start, sustain end, release, final
    std::optional<int> dragIndex;
};


class WaveformDisplayGraphics : public juce::Component
{
public:
    WaveformDisplayGraphics()
    {
        calculateEnvelopeSegments();
    }
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();

        juce::Path bgFill;
        bgFill.addRoundedRectangle(bounds, 5.0f);
        g.setColour(juce::Colour(36, 159, 208));
        g.fillPath(bgFill);
        
        float x = bounds.getX();
        float y = bounds.getY();
        float height = bounds.getHeight();
        float graphicWidth = bounds.getWidth() * 0.9f;
        float graphicHeight = bounds.getHeight() * 0.9f;
        float widthMargin = bounds.getWidth() * 0.05f;
        float heightMargin = bounds.getHeight() * 0.05f;

        int envelopeSegments = 72;
        int domainResolution = 128;
        
        juce::Path graphicLines;
        float widthIncrement = graphicWidth/domainResolution;
        float heightIncrement = graphicHeight/envelopeSegments;

        for (int j = 0; j < envelopeSegments; j++){
            int k = envelopeSegments - j;
            
            float amp2 = op[2].generateAmplitude(k);
            float amp1 = op[1].generateAmplitude(k);
            float amp0 = op[0].generateAmplitude(k);

            
            float heightScaled = y + heightMargin + heightIncrement * j;
            graphicLines.startNewSubPath(x + widthMargin, heightScaled + height * 0.005f);

            for (int i = 0; i < domainResolution; i++)
            {
             //   float sin2 = amp2 * op[1].modIndex * fastSin.sin((i/40.7f) * op[2].ratio);
                
                float sin2Phase = fmodf(((i/40.7f) * op[1].ratio) + 0.0f, 6.28318f);
                float sin2 = amp2 * op[1].modIndex * fastSin.sin(sin2Phase);
                
                float sin1Phase = fmodf(((i/40.7f) * op[1].ratio) + sin2, 6.28318f);
                float sin1 = amp1 * op[0].modIndex * fastSin.sin(sin1Phase);
                
                float sin0Phase = fmodf(((i/40.7f) * op[0].ratio) + sin1, 6.28318f);
                float sin = amp0 * fastSin.sin(sin0Phase);
                
                graphicLines.lineTo(x + widthMargin + widthIncrement * i,
                                    (heightScaled + height/envelopeSegments) + sin * height * 0.005f);
            }
        }
        
        graphicLines = graphicLines.createPathWithRoundedCorners(10.0f);
        g.setColour(juce::Colour(255, 255, 255));
        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(graphicLines, strokeType);
    }
    
    
    void resized() override {};
    
    void setFMParameter(int index, float ratio, float fixed, bool isRatio, float modIndex)
    {
        op[index].ratio = ratio;
        op[index].fixed = fixed;
        op[index].modIndex = modIndex/10.0f;
        op[index].isRatio = isRatio;
    }
    
    void setEnvelope(int index, float attack, float decay, float sustain, float release)
    {
        op[index].attack = attack * 1000.0f;
        op[index].decay = decay * 1000.0f;
        op[index].sustain = sustain / 10.0f;
        op[index].release = release * 1000.0f;
        
        calculateEnvelopeSegments();
        repaint();
    }
    
    void calculateEnvelopeSegments()
    {
        for (int index = 0; index < 4; index++)
        {
            float attackDecayTime = (op[index].attack + op[index].decay);
            op[index].attackSegment = (op[index].attack/attackDecayTime) * 36;
            op[index].decaySegment = (op[index].decay/attackDecayTime) * 36;

            float releaseTime = op[index].release/20.0f; // 20 total release segment
            op[index].releaseSegment = releaseTime;
        }
    }
    
private:
    int envelopeSegments = 72;
    std::vector<float> segmentAmplitude;
    juce::dsp::FastMathApproximations fastSin;
    struct operatorValues
    {
        float ratio, fixed, modIndex;
        bool isRatio;
        float attack = 3000.0f, decay = 1000.0f, sustain = 1.0f, release = 5000.0f;
        float attackSegment, decaySegment, releaseSegment;
        
        float generateAmplitude(float segmentIndex)
        {
            float amplitude = 0.0f;

            if (segmentIndex <= attackSegment && attackSegment > 0)
            {
                amplitude = segmentIndex / attackSegment;
            }
            else if (segmentIndex > attackSegment && segmentIndex <= attackSegment + decaySegment && decaySegment > 0)
            {
                int decayIndex = segmentIndex - attackSegment;
                float decayProgress = decayIndex / decaySegment;
                amplitude = 1.0f + decayProgress * (sustain - 1.0f); // linear interpolation from 1 to sustain
            }
            else if (segmentIndex > attackSegment + decaySegment && segmentIndex <= attackSegment + decaySegment + releaseSegment && releaseSegment > 0)
            {
                int releaseIndex = segmentIndex - attackSegment - decaySegment;
                float releaseProgress = releaseIndex / releaseSegment;
                amplitude = sustain * (1.0f - releaseProgress);
            }
            else if (segmentIndex > attackSegment + decaySegment + releaseSegment)
            {
                amplitude = 0.0f;
            }
            else
            {
                amplitude = sustain;
            }
            return amplitude;
        }
    };
    
    std::array<operatorValues, 4> op;
};
