/*
  ==============================================================================

    VoiceControlsLayout.h
    Created: 1 Sep 2025 5:16:37pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GraphicsUtility.h"
#include "PluginProcessor.h"
#include "DialLookAndFeel.h"

enum class VoiceControlsLAF { Gain, VoiceCount, VoiceSlew };

class VoiceControlsLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VoiceControlsLookAndFeel(VoiceControlsLAF l) : lookAndFeel(l) {}
    
    void drawLevelMeter(juce::Graphics& g, float x, float y, float width, float height, float pos);
    void drawVoiceCount(juce::Graphics& g, float x, float y, float size, float pos);
    void drawVoiceSlew(juce::Graphics& g, float x, float y, float size, float pos);

private:
    VoiceControlsLAF lookAndFeel;
    
};

class VoiceControlsInterface : public juce::Component, juce::Timer, GraphicsHelper
{
public:
    VoiceControlsInterface(FledgeAudioProcessor& p);
    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    void setLabel(juce::Label &l, juce::String labelText, float size);
    void timerCallback() override;

    int index;
    
    VoiceControlsLookAndFeel
    gainLAF { VoiceControlsLAF::Gain },
    voiceCountLAF { VoiceControlsLAF::VoiceCount },
    voiceSlewLAF { VoiceControlsLAF::VoiceSlew };

    juce::Slider
    gainSlider,
    voiceCountSlider,
    voiceSlewSlider;
    
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    ratioGraphicAttachment,
    amplitudeGraphicAttachment;

    juce::Label
    gainLabel,
    voiceCountLabel,
    voiceSlewLabel;
    
    std::unique_ptr<EditableTextBoxSlider>
    voiceCountTextSlider,
    voiceSlewTextSlider;

    FledgeAudioProcessor& audioProcessor;
};
