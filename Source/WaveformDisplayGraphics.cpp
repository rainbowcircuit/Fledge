/*
  ==============================================================================

    WaveformDisplayGraphics.cpp
    Created: 8 Aug 2025 2:32:14pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "WaveformDisplayGraphics.h"

OperatorDisplayGraphics::OperatorDisplayGraphics(FledgeAudioProcessor& p) : audioProcessor(p)
{
    const auto params = audioProcessor.getParameters();
    for (auto param : params){
        param->addListener(this);
    }
}

OperatorDisplayGraphics::~OperatorDisplayGraphics()
{
    const auto params = audioProcessor.getParameters();
    for (auto param : params){
        param->removeListener(this);
    }
}

void OperatorDisplayGraphics::setIndex(int index)
{
    this->index = index;
}

void OperatorDisplayGraphics::paint(juce::Graphics &g) 
{
    bounds = getLocalBounds().toFloat();
    fillControlPanel(g, bounds);

    bounds.reduce(5, 5);
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.setColour(Colors::mainColors[index]);
    
    juce::Path bgWaveform = waveformPath(g, x + width * 0.05f,
                                       y + height * 0.075f,
                                       width * 0.9f,
                                       height * 0.775f,
                                       1.0f, 1.0f, 0.0f);
    g.setColour(Colors::greyColor);
    g.strokePath(bgWaveform, strokeType);

    
    juce::Path fgWaveform = waveformPath(g, x + width * 0.05f,
                                       y + height * 0.075f,
                                       width * 0.9f,
                                       height * 0.775f,
                                       ratio, amplitude, phase);
    g.setColour(Colors::mainColors[index]);
    g.strokePath(fgWaveform, strokeType);
}

void OperatorDisplayGraphics::resized()
{
    macroPosition.x = (audioProcessor.params->ratio[index]->getSafe()/20.0f) * bounds.getWidth();
    macroPosition.y = (1.0f - audioProcessor.params->amplitude[index]->getSafe()/100.0f) * bounds.getHeight();
}


juce::Path OperatorDisplayGraphics::waveformPath(juce::Graphics &g, float x, float y, float width, float height, float freq, float amp, float phase)
{
    juce::Path graphicPath;
    graphicPath.startNewSubPath(x, y + height/2);
    
    float twopi = juce::MathConstants<float>::twoPi;
    int domainResolution = 128;
    float widthIncrement = width/domainResolution;
    float phaseScale = domainResolution/twopi;
    phase = (phase/100.0f) * twopi;
    
    float sinStart = std::sin((0/phaseScale) * freq + phase);
    graphicPath.startNewSubPath(x + widthIncrement * 0, (y + height/2) + (height * sinStart/2) * amp);

    for (int i = 1; i < domainResolution; i++)
    {
        float sin = std::sin((i/phaseScale) * freq + phase);
        graphicPath.lineTo(x + widthIncrement * i, (y + height/2) + (height * sin/2) * amp);
    }

    graphicPath = graphicPath.createPathWithRoundedCorners(4.0f);
    return graphicPath;
}


void OperatorDisplayGraphics::setRatioAndAmplitude(float ratio, float fixed, float amplitude, bool isRatio, float phase)
{
    this->ratio = ratio;
    this->fixed = fixed;
    this->amplitude = amplitude/100.0f;
    this->phase = phase;
    repaint();
}

void OperatorDisplayGraphics::setParameter(float x, float y)
{
    float amplitude = juce::jlimit(0.0f, 1.0f, 1.0f - (y/100.0f));
    audioProcessor.params->apvts.getParameter("amplitude" + juce::String(index))->setValueNotifyingHost(amplitude);
    
    float ratio = juce::jlimit(0.0f, 1.0f, x/500.0f);
    audioProcessor.params->apvts.getParameter("ratio" + juce::String(index))->setValueNotifyingHost(ratio);
}

void OperatorDisplayGraphics::mouseDrag(const juce::MouseEvent& m)
{
    auto mouse = m.getPosition().toFloat();
    macroPosition = mouse;
    setParameter(mouse.x, mouse.y);
    repaint();
}

void OperatorDisplayGraphics::parameterValueChanged (int parameterIndex, float newValue)
{
    newValueAtomic.store(newValue);
    parameterIndexAtomic.store(parameterIndex);
    triggerAsyncUpdate();
}

void OperatorDisplayGraphics::handleAsyncUpdate()
{
    float newValue = newValueAtomic.load();
    int parameterIndex = parameterIndexAtomic.load();
    juce::String newParameterID;
    
    if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[parameterIndex]))
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            newParameterID = param->paramID;
        }
    }
    
    if (newParameterID == "ratio" + juce::String(index))
    {
        macroPosition.x = newValue * bounds.getWidth();
    } else if (newParameterID == "amplitude" + juce::String(index))
    {
        macroPosition.y = (1.0f - newValue) * bounds.getHeight();
    }
}

void OperatorDisplayGraphics::timerCallback()
{
    auto bounds = getLocalBounds().toFloat();
    auto mouse = getMouseXYRelative().toFloat();
    
    if (bounds.contains(mouse))
    {
        setMouseCursor(juce::MouseCursor::UpDownLeftRightResizeCursor);
    }
}
