#pragma once
#include <JuceHeader.h>
#include <cmath>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

//Takuma your waveforms look like a butt


class LevelMeterGraphics : public juce::Component
{
    void paint(juce::Graphics &g) override
    {
        
    }
    
    void resized() override {}

    void setLevel(float level)
    {
        this->level = level;
    }
    
private:
    float level = 0.0f;
};

class OperatorDisplayGraphics : public juce::Component
{
public:
    void setIndex(int index)
    {
        this->index = index;
    }
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        juce::Path bgFill;
        bgFill.addRoundedRectangle(bounds, 5.0f);
        g.setColour(juce::Colour(12, 10, 11));

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
        g.setColour(Colors::mainColors[index]);
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
    int index;
    float ratio, fixed, modIndex;
};



class EnvelopeDisplayGraphics : public juce::Component
{
public:
    EnvelopeDisplayGraphics(FledgeAudioProcessor &p, int index) : audioProcessor(p)
    {
        this->index = index;
    }
    
    void setIndex(int index)
    {
    }

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
        // unadjusted background
        drawSegment(g, x + widthMargin, y + heightMargin, width, height, false);

        // adjusted foreground
        drawSegment(g, x + widthMargin, y + heightMargin, width, height, true);
        
        for (int i = 0; i < 5; i++)
        {
            pointsGlobalAdjusted[i].drawHandles(g);
        }
    }
    
    void resized() override
    {
        calculateSegment();
        pointsNoAdjusted[0].yAdjustOnly = false;
        pointsNoAdjusted[1].yAdjustOnly = false;
        pointsNoAdjusted[2].yAdjustOnly = false;
        pointsNoAdjusted[3].yAdjustOnly = true;
        pointsNoAdjusted[4].yAdjustOnly = false;

        pointsGlobalAdjusted[0].yAdjustOnly = false;
        pointsGlobalAdjusted[1].yAdjustOnly = false;
        pointsGlobalAdjusted[2].yAdjustOnly = false;
        pointsGlobalAdjusted[3].yAdjustOnly = true;
        pointsGlobalAdjusted[4].yAdjustOnly = false;
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

        pointsNoAdjusted[0].coords = { x + widthMargin, y + height + heightMargin }; // Bottom (0)
        pointsNoAdjusted[1].coords = { x + widthMargin + width * attackPct, y + heightMargin }; // Top (1.0)
        pointsNoAdjusted[2].coords = { x + widthMargin + width * (attackPct + decayPct), y + heightMargin + height * (1.0f - sustain) }; // Sustain level
        pointsNoAdjusted[3].coords = { x + widthMargin + width * (attackPct + decayPct + sustainPct), y + heightMargin + height * (1.0f - sustain) }; // Same sustain level
        pointsNoAdjusted[4].coords = { x + widthMargin + width, y + height + heightMargin }; // Bottom (0)
        repaint();

        
        pointsGlobalAdjusted[0].coords = { x + widthMargin, y + height + heightMargin }; // Bottom (0)
        pointsGlobalAdjusted[1].coords = { x + widthMargin + width * attackAdjPct, y + heightMargin }; // Top (1.0)
        pointsGlobalAdjusted[2].coords = { x + widthMargin + width * (attackAdjPct + decayAdjPct), y + heightMargin + height * (1.0f - sustainLevelAdjusted) }; // Sustain level
        pointsGlobalAdjusted[3].coords = { x + widthMargin + width * (attackAdjPct + decayAdjPct + sustainPct), y + heightMargin + height * (1.0f - sustainLevelAdjusted) }; // Same sustain level
        pointsGlobalAdjusted[4].coords = { x + widthMargin + width, y + height + heightMargin }; // Bottom (0)
        repaint();
    }
    
  
    void setEnvelope(float attack, float decay, float sustain, float release, float attackAdj, float decayAdj, float sustainAdj, float releaseAdj)
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
        
        if (adrAdjSum > 0.0f)
        {
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
    
    void drawSegment(juce::Graphics &g, float x, float y, float width, float height, bool drawAdjusted)
    {
        auto points = drawAdjusted ? pointsGlobalAdjusted : pointsNoAdjusted;
        auto envelopeColor = drawAdjusted ? Colors::mainColors[index] : juce::Colour(130, 130, 130);
        
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
        
        if (drawAdjusted){
            g.setColour(Colors::mainColors[index].withAlpha((float)0.15f));
            g.fillPath(envelopePath);
        }
        
        g.setColour(envelopeColor);
        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(envelopePath, strokeType);
        
        
    }

    void mouseDown(const juce::MouseEvent &m) override
    {
        auto mouse = m.getPosition().toFloat();
        for (int i = 1; i < 5; i++) // ignore init segment
        {
            if (pointsGlobalAdjusted[i].isOver(mouse))
            {
                dragIndex = i;
                dragStartPoint = mouse;
                
                auto segmentParameterID = (getSegmentParamID(*dragIndex));
                initialParamValue = audioProcessor.apvts.getParameter(segmentParameterID)->getValue();
            }
        }
    }
    
    void mouseDrag(const juce::MouseEvent &m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        float sensitivity = 0.01f;

        if (dragIndex.has_value())
        {
            if (pointsGlobalAdjusted[*dragIndex].yAdjustOnly)
            {
                float deltaY = mousePoint.y - dragStartPoint.y;
                float newValue = juce::jlimit(0.0f, 1.0f, initialParamValue + (-deltaY * sensitivity));
                
                setEnvelopeParam(*dragIndex, newValue);
            } else {
                float deltaX = mousePoint.x - dragStartPoint.x;
                float newValue = juce::jlimit(0.0f, 1.0f, initialParamValue + (-deltaX * sensitivity));
                
                setEnvelopeParam(*dragIndex, newValue);
            }
            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent &m) override
    {
        dragIndex.reset();
    }
    
    juce::String getSegmentParamID(int segmentDragged)
    {
        juce::String segmentParameterID;
        switch(segmentDragged)
        {
            case 1:
                segmentParameterID = "attack" + juce::String(index);
                break;
            case 2:
                segmentParameterID = "decay" + juce::String(index);
                break;
            case 3:
                segmentParameterID = "sustain" + juce::String(index);
                break;
            case 4:
                segmentParameterID = "release" + juce::String(index);
                break;
        }
        return segmentParameterID;
    }
    
    void setEnvelopeParam(int segmentDragged, float adjustAmount)
    {
        auto segmentParameterID = getSegmentParamID(segmentDragged);
        auto paramRange = audioProcessor.apvts.getParameterRange(segmentParameterID);
        
        audioProcessor.apvts.getParameter(segmentParameterID)->setValueNotifyingHost(adjustAmount);
    }

    
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
    
    std::array<Handle, 5> pointsNoAdjusted;
    std::array<Handle, 5> pointsGlobalAdjusted;

    std::optional<int> dragIndex;
    FledgeAudioProcessor& audioProcessor;
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

            for (int i = 1; i <= domainResolution; i++)
            {
                
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
        g.setColour(Colors::mainColors[0]);
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
        calculateEnvelopeSegments();
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
