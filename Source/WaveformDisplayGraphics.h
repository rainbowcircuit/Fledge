/*
  ==============================================================================

    WaveformDisplayGraphics.h
    Created: 8 Aug 2025 2:32:14pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GraphicsUtility.h"
#include "LookAndFeel.h"

class OperatorDisplayGraphics : public juce::Component, juce::AudioProcessorParameter::Listener, juce::AsyncUpdater, juce::Timer, GraphicsHelper
{
public:
    OperatorDisplayGraphics(FledgeAudioProcessor& p);
    ~OperatorDisplayGraphics();
    
    //==============================================================================
    void setIndex(int index);
    void paint(juce::Graphics &g) override;
    void resized() override {}
    juce::Path waveformPath(juce::Graphics &g, float x, float y, float width, float height, float freq, float amp, float phase);

    //==============================================================================
    void setRatioAndAmplitude(float ratio, float fixed, float amplitude, bool isRatio, float phase);
    void setParameter(float x, float y);
    void mouseDrag(const juce::MouseEvent& m) override;

    
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
        void handleAsyncUpdate() override;


private:
    
    void timerCallback() override;

    juce::Rectangle<float> bounds;
    juce::Point<float> macroPosition;
    
    std::atomic<float> newValueAtomic;
    std::atomic<int> parameterIndexAtomic;
    
    int index;
    float ratio, fixed, amplitude, phase;
    
    FledgeAudioProcessor& audioProcessor;
};
