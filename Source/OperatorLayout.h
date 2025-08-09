/*
  ==============================================================================

    OperatorLayout.h
    Created: 8 Aug 2025 2:31:50pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DialLookAndFeel.h"
#include "WaveformDisplayGraphics.h"
#include "EnvelopeGraphics.h"

class OperatorInterface : public juce::Component, juce::Timer, GraphicsHelper
{
public:
    OperatorInterface(FledgeAudioProcessor& p, int index);
    void setIndex(int index);
    void paint(juce::Graphics &g) override;
    void resized() override;
    //==============================================================================

    std::unique_ptr<OperatorDisplayGraphics> opGraphics;
    std::unique_ptr<EnvelopeDisplayGraphics> envGraphics;

private:
    void setLabel(juce::Label &l, juce::String labelText, float size);
    void timerCallback() override;

    int index;
    
    juce::Label ratioLabel,
    fixedLabel,
    amplitudeLabel,
    phaseLabel,
    attackLabel,
    decayLabel,
    sustainLabel,
    releaseLabel;
    
    std::unique_ptr<EditableTextBoxSlider> ratioSlider,
    fixedSlider,
    amplitudeSlider,
    phaseSlider,
    attackSlider,
    decaySlider,
    sustainSlider,
    releaseSlider;

    FledgeAudioProcessor& audioProcessor;
};
