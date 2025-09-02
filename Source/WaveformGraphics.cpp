/*
  ==============================================================================

    WaveformGraphics.cpp
    Created: 8 Aug 2025 10:45:15pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "WaveformGraphics.h"
WaveformDisplayGraphics::WaveformDisplayGraphics()
{
    calculateEnvelopeSegments();
    
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 72; k++) {
            ampSmooth[i][k].reset(5);
        }
    }
}

void WaveformDisplayGraphics::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    float twopi = juce::MathConstants<float>::twoPi;
    float x = bounds.getX();
    float y = bounds.getY();
    float height = bounds.getHeight();
    float graphicWidth = bounds.getWidth() * 0.9f;
    float graphicHeight = bounds.getHeight() * 0.85f;
    float widthMargin = bounds.getWidth() * 0.05f;
    float heightMargin = bounds.getHeight() * 0.075f;

    float phaseScale = domainResolution/twopi;

    juce::Path graphicLines;
    float widthIncrement = graphicWidth/domainResolution;
    float heightIncrement = graphicHeight/envelopeSegments;

    for (int j = 0; j <= envelopeSegments; j++){
        int k = envelopeSegments - j - 1;
        
        if (k > 0 && k <= 72) {
            float amp3 = ampSmooth[3][k].getNextValue();
            float amp2 = ampSmooth[2][k].getNextValue();
            float amp1 = ampSmooth[1][k].getNextValue();
            float amp0 = ampSmooth[0][k].getNextValue();
            
            float heightScaled = y + heightMargin + heightIncrement * j;
            
            float sin0Phase = fmodf(((0/phaseScale) * op[0].ratio * 2.0f) + op[0].phase * twopi, twopi);
            float sin = amp0 * fastSin.sin(sin0Phase);

            graphicLines.startNewSubPath(x + widthMargin, (heightScaled + height/envelopeSegments) + sin * height * 0.005f);
            for (int i = 1; i <= domainResolution; i++)
            {
                float op3Sin = fastSin.sin(fmodf(((i/phaseScale) * op[3].ratio)
                                                 + (op[3].phase * twopi)
                                                 + ((op0Phase * op[3].gain[0])
                                                    + (op1Phase * op[3].gain[1])
                                                    + (op2Phase * op[3].gain[2])
                                                    + (op3Phase * op[3].gain[3])) * 8.0f, twopi));
                op3Phase = amp3 * op[3].amplitude * op3Sin;
                
                float op2Sin = fastSin.sin(fmodf(((i/phaseScale) * op[2].ratio)
                                                 + (op[2].phase * twopi)
                                                 + ((op0Phase * op[2].gain[0])
                                                    + (op1Phase * op[2].gain[1])
                                                    + (op2Phase * op[2].gain[2])
                                                    + (op3Phase * op[2].gain[3])) * 8.0f, twopi));
                op2Phase = amp2 * op[2].amplitude * op2Sin;
                
                float op1Sin = fastSin.sin(fmodf(((i/phaseScale) * op[1].ratio)
                                                 + (op[1].phase * twopi)
                                                 + ((op0Phase * op[1].gain[0])
                                                    + (op1Phase * op[1].gain[1])
                                                    + (op2Phase * op[1].gain[2])
                                                    + (op3Phase * op[1].gain[3])) * 8.0f, twopi));
                op1Phase = amp1 * op[1].amplitude * op1Sin;
                
                float op0Sin = fastSin.sin(fmodf(((i/phaseScale) * op[0].ratio)
                                                 + (op[0].phase * twopi)
                                                 + ((op0Phase * op[0].gain[0])
                                                    + (op1Phase * op[0].gain[1])
                                                    + (op2Phase * op[0].gain[2])
                                                    + (op3Phase * op[0].gain[3])) * 8.0f, twopi));
                op0Phase = amp0 * op[0].amplitude * op0Sin;
                
                float outputPhase = ((op0Phase * outputGain[0])
                                     + (op1Phase * outputGain[1])
                                     + (op2Phase * outputGain[2])
                                     + (op3Phase * outputGain[3]));
                
                outputPhase = std::tanh(outputPhase);
                
                graphicLines.lineTo(x + widthMargin + widthIncrement * i,
                                    (heightScaled + height/envelopeSegments) + outputPhase * height * 0.05f);
            }
        }
    }
    
    graphicLines = graphicLines.createPathWithRoundedCorners(40.0f);
    g.setColour(Colors::mainColors[0]);
    juce::PathStrokeType strokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.strokePath(graphicLines, strokeType);
}

void WaveformDisplayGraphics::setGainCoefficients(int index, int gainIndex, int outputGainIndex)
{
    outputGain = toBinary4(outputGainIndex);
    switch(index){
        case 0:
            op[0].gain = toBinary4(gainIndex);
            break;
        case 1:
            op[1].gain = toBinary4(gainIndex);
            break;
        case 2:
            op[2].gain = toBinary4(gainIndex);
            break;
        case 3:
            op[3].gain = toBinary4(gainIndex);
            break;
    }
    
    repaint();
}

void WaveformDisplayGraphics::setEnvelope(int index, float attack, float decay, float sustain, float release)
{
    op[index].attack = attack;
    op[index].decay = decay;
    op[index].sustain = sustain/100.0f;
    op[index].release = release;
    calculateEnvelopeSegments();
    calculateAmplutude();
    repaint();
}

void WaveformDisplayGraphics::setFMParameter(int index, float ratio, float fixed, bool isRatio, float amplitude, float phase)
{
    op[index].ratio = ratio;
    op[index].fixed = fixed;
    op[index].amplitude = amplitude/100.0f;
    op[index].isRatio = isRatio;
    op[index].phase = phase/100.0f;
    calculateAmplutude();
    repaint();
}

void WaveformDisplayGraphics::calculateEnvelopeSegments()
{
    for (int index = 0; index < 4; index++)
    {
        float attackDecayTime = (op[index].attack + op[index].decay);
        op[index].attackSegment = (op[index].attack/attackDecayTime) * 36;
        op[index].decaySegment = (op[index].decay/attackDecayTime) * 36;
        float releaseClamped = juce::jlimit(0.0f, 20.0f, op[index].release);
        op[index].releaseSegment = std::pow(releaseClamped/20.0f, 0.5f) * 24;
    }
}

void WaveformDisplayGraphics::calculateAmplutude()
{
    for (int j = 0; j < envelopeSegments; j++){
        int k = envelopeSegments - j;
        ampSmooth[3][k].setTargetValue(op[3].generateAmplitude(k));
        ampSmooth[2][k].setTargetValue(op[2].generateAmplitude(k));
        ampSmooth[1][k].setTargetValue(op[1].generateAmplitude(k));
        ampSmooth[0][k].setTargetValue(op[0].generateAmplitude(k));
    }
}

