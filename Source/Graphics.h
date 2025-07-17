#pragma once
#include <JuceHeader.h>
#include <cmath>
#include "LookAndFeel.h"

//Takuma your waveforms look like a butt

class OperatorDisplayGraphics : public juce::Component
{
public:
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        juce::Path bgFill;
        bgFill.addRoundedRectangle(bounds, 5.0f);
        g.setColour(juce::Colour(12, 10, 11));
      //  g.fillPath(bgFill);

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();

        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        
        float bgAmpScale = 1.0f;
        float fgAmpScale = modIndex;
        if (modIndex >= 1.0f)
        {
            bgAmpScale = (9.0f - (modIndex - 1.0f))/9.0f;
            fgAmpScale = 1.0f;
        }
        
        juce::Path bgWaveform = waveformPath(g, x + width * 0.05f, y + height * 0.125f, width * 0.9f, height * 0.75f, 1.0f, bgAmpScale);
        g.setColour(juce::Colour(80, 82, 81));
        g.strokePath(bgWaveform, strokeType);
        
        
        juce::Path fgWaveform = waveformPath(g, x + width * 0.05f, y + height * 0.125f, width * 0.9f, height * 0.75f, ratio, fgAmpScale);
        g.setColour(juce::Colour(90, 224, 184));
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
    
    void resized() override {}
    
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
    EnvelopeDisplayGraphics() {}
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() * 0.9f;
        float height = bounds.getHeight() * 0.9f;
        float widthMargin = bounds.getWidth() * 0.05f;
        float heightMargin = bounds.getHeight() * 0.05f;
        
        calculateSegment();
        drawSegment(g, x + widthMargin, y + heightMargin, width, height);
        
        for (int i = 0; i < 5; i++)
        {
            points[i].drawHandles(g);
        }
    }
    
    void resized() override
    {
        calculateSegment();
        
        points[0].yAdjustOnly = true;
        points[1].yAdjustOnly = true;
        points[2].yAdjustOnly = true;
        points[3].yAdjustOnly = true;
        points[4].yAdjustOnly = true;
      //  points[5].yAdjustOnly = false;
     //   points[6].yAdjustOnly = false;
       // points[7].yAdjustOnly = false;
    }
    
    
    float calculateScaledPercentage(float segment, float total)
    {
        if (total == 0.0f)
            return 0.0f; // avoid divide-by-zero

        float proportion = (segment / total);
        float scale = 80.0f * 0.01f;

        return proportion * scale;
    }

    void calculateSegment()
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() * 0.9f;
        float height = bounds.getHeight() * 0.9f;
        float widthMargin = bounds.getWidth() * 0.05f;
        float heightMargin = bounds.getHeight() * 0.05f;

        points[0].coords = { x + widthMargin, y + height + heightMargin }; // initial
        points[1].coords = { points[0].coords.x + width * attackPct, y + heightMargin }; // peak
        points[2].coords = { points[1].coords.x + width * decayPct, y + heightMargin + height * 0.3f}; // sustain start
        points[3].coords = { points[2].coords.x + width * sustainPct, y + heightMargin + height * 0.3f }; // sustain end
        points[4].coords = { points[3].coords.x + width * releasePct, y + heightMargin + height }; // end of envelope
        repaint();
    }
    
    void setEnvelope(float attack, float decay, float sustain, float release)
    {
        this->attack = attack;
        this->decay = decay;
        this->sustain = sustain;
        this->release = release;
        
        float sustainPercent = 0.25f;
        float remainderPercent = 1.0f - sustainPercent;
        float adrSum = attack + decay + release;

        if (adrSum > 0.0f) {
            attackPct  = (attack / adrSum) * remainderPercent;
            decayPct   = (decay / adrSum) * remainderPercent;
            releasePct = (release / adrSum) * remainderPercent;
            
        } else {
            float evenShare = remainderPercent / 3.0f;
            attackPct  = evenShare;
            decayPct   = evenShare;
            releasePct = evenShare;
        }
        
        sustainPct = sustainPercent;
    }
    
    void drawSegment(juce::Graphics &g, float x, float y, float width, float height)
    {
        juce::Path envelopePath;
        envelopePath.startNewSubPath(points[0].coords);
        envelopePath.lineTo(points[1].coords);
        envelopePath.lineTo(points[2].coords);
        envelopePath.lineTo(points[3].coords);
        envelopePath.lineTo(points[4].coords);

        g.setColour(juce::Colour(90, 224, 184));
        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(envelopePath, strokeType);
        
    }

    void mouseDown(const juce::MouseEvent &m) override
    {
        auto mouse = m.getPosition().toFloat();
        for (int i = 0; i < 8; i++)
        {
            if (points[i].isOver(mouse))
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
            if (!points[*dragIndex].yAdjustOnly)
            {
                points[*dragIndex].coords = { mouse.x, mouse.y };
            } else {
                points[*dragIndex].coords.y = mouse.y;
            } 
            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent &m) override
    {
        dragIndex.reset();
    }
    
private:
    float attackPct, decayPct, releasePct, sustainPct;
    float attack, decay, sustain, release;
    float attackSegment, decaySegment, sustainSegment, releaseSegment;
    
    struct Handle
    {
        juce::Point<float> coords;
        bool isMouseOver;
        bool yAdjustOnly;
        
        bool isOver(juce::Point<float>& m)
        {
            juce::Rectangle point(coords.x - 5.0f, coords.y - 5.0f, 10.0f, 10.0f);
            return (point.contains(m));
        }
        
        void drawHandles(juce::Graphics &g)
        {
            juce::Path handlePath;
            handlePath.addRoundedRectangle(coords.x - 3.0f, coords.y - 3.0f, 6.0f, 6.0f, 1.5f);
            g.setColour(juce::Colour(200, 200, 200));  // change via hover
            g.strokePath(handlePath, juce::PathStrokeType(1.0f));
        }
    };
    
    std::array<Handle, 8> points;
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
            
            float sin0Phase = fmodf(((0/40.7f) * op[0].ratio * 2.0f), 6.28318f);
            float sin = amp0 * fastSin.sin(sin0Phase);

            graphicLines.startNewSubPath(x + widthMargin, (heightScaled + height/envelopeSegments) + sin * height * 0.005f);

            for (int i = 0; i < domainResolution; i++)
            {
             //   float sin2 = amp2 * op[1].modIndex * fastSin.sin((i/40.7f) * op[2].ratio);
                
                float sin2Phase = fmodf(((i/40.7f) * op[1].ratio * 2.0f) + 0.0f, 6.28318f);
                float sin2 = amp2 * op[1].modIndex * fastSin.sin(sin2Phase);
                
                float sin1Phase = fmodf(((i/40.7f) * op[1].ratio * 2.0f) + sin2, 6.28318f);
                float sin1 = amp1 * op[0].modIndex * fastSin.sin(sin1Phase);
                
                float sin0Phase = fmodf(((i/40.7f) * op[0].ratio * 2.0f) + sin1, 6.28318f);
                float sin = amp0 * fastSin.sin(sin0Phase);
                
                graphicLines.lineTo(x + widthMargin + widthIncrement * i,
                                    (heightScaled + height/envelopeSegments) + sin * height * 0.005f);
            }
        }
        
        graphicLines = graphicLines.createPathWithRoundedCorners(10.0f);
        g.setColour(juce::Colour(90, 224, 184));
        juce::PathStrokeType strokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(graphicLines, strokeType);
    }
    
    
    void resized() override {};
    
    void setEnvelope(int index, float attack, float decay, float sustain, float release)
    {
        op[index].attack = attack;
        op[index].decay = decay;
        op[index].sustain = sustain/10.0f;
        op[index].release = release;
        repaint();
    }
    
    void setFMParameter(int index, float ratio, float fixed, bool isRatio, float modIndex)
    {
        op[index].ratio = ratio;
        op[index].fixed = fixed;
        op[index].modIndex = modIndex/10.0f;
        op[index].isRatio = isRatio;
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
