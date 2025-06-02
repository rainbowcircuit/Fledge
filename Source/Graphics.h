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

/*
class OperatorControlGraphics : public juce::Component
{
public:
    void paint(juce::Graphics &g) override;
    void resized() override;
    
    void setParameters(bool isRatio, float ratio, float fixed, float modIndex);
    
    void mouseDown(const juce::MouseEvent &m) override;
    void mouseDrag(const juce::MouseEvent &m) override;
    void mouseUp(const juce::MouseEvent &m) override;
    
private:
    bool isRatio;
    float freq = 1.0f, amount = 1.0f; // testVariables
    
    float ratio, fixed, modIndex;
};
*/


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

            
            float height = bounds.getY() + heightIncrement * j;
            graphicLines.startNewSubPath(bounds.getX(), height + bounds.getHeight()/envelopeSegments);

            for (int i = 0; i < domainResolution; i++)
            {
                

                float sin2 = amp2 * op[1].modIndex * fastSin.sin((i/40.7f) * op[2].ratio);
                float sin1 = amp1 * op[0].modIndex * fastSin.sin(((i/40.7f) * op[1].ratio) + sin2);
                float sin = amp0 * fastSin.sin(((i/40.7f) * op[0].ratio) + sin1);
                
                
                graphicLines.lineTo(bounds.getX() + widthIncrement * i,
                                    (height + bounds.getHeight()/envelopeSegments) + sin * bounds.getHeight()/envelopeSegments);
                
            }
        }
        graphicLines = graphicLines.createPathWithRoundedCorners(4.0f);
        g.setColour(juce::Colour(255, 255, 255));
        g.strokePath(graphicLines, juce::PathStrokeType(1));
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
        op[index].sustain = sustain / 100.0f;
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
        int attackSegment, decaySegment, releaseSegment;
        
        float generateAmplitude(int segmentIndex)
        {
            float amplitude;
            for (int i = 0; i < 72; i++)
            {
                if (segmentIndex <= attackSegment){
                    float attackIncr = 1.0f/attackSegment;
                    amplitude = attackIncr * segmentIndex;
                    
                } else if (segmentIndex > attackSegment && segmentIndex <= attackSegment + decaySegment){
                    float decayIncr = 1.0f/decaySegment;
                    
                    segmentIndex = (decaySegment - segmentIndex) + attackSegment; // offset out the attacksegment
                    amplitude = decayIncr * segmentIndex;
                    
                } else if (segmentIndex > attackSegment + decaySegment) {
                    float releaseIncr = sustain/releaseSegment;
                    
                    segmentIndex = (releaseSegment - segmentIndex) + attackSegment + decaySegment;
                    amplitude = releaseIncr * segmentIndex;
                }
            }
            return amplitude;
        }
    };
    
    std::array<operatorValues, 4> op;
};
