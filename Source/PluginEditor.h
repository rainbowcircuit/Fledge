/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UserInterface.h"
#include "ButtonLookAndFeel.h"
#include "LookAndFeel.h"
#include "LevelMeter.h"
#include "GraphicsUtility.h"
#include "OperatorLayout.h"
#include "AlgorithmGraphics.h"
#include "AlgorithmSelector.h"
#include "PresetLayout.h"
#include "WaveformGraphics.h"

class FledgeAudioProcessorEditor : public juce::AudioProcessorEditor, juce::Timer, juce::Button::Listener, GraphicsHelper
{
public:
    FledgeAudioProcessorEditor (FledgeAudioProcessor&);
    ~FledgeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void buttonClicked(juce::Button* buttonClicked) override;

private:
    ButtonLookAndFeel showWaveLAF { 3 }, showAlgoLAF { 4 }, showMacroLAF { 5 };
    
    std::array<std::unique_ptr<OperatorInterface>, 4>  opInterface;
    std::unique_ptr<PresetInterface>  presetInterface;
    std::unique_ptr<MacroControlsInterface> macroInterface;
    
    juce::Slider volumeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

    
    juce::TextButton showWaveformButton, showAlgorithmButton, showMacrosButton;
    WaveformDisplayGraphics waveformDisplay;
    std::unique_ptr<AlgorithmGraphics> algorithmGraphics;
    std::unique_ptr<AlgorithmSelectInterface> algorithmSelector;
    
    std::unique_ptr<LevelMeter> outputLevelMeter;

    FledgeAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessorEditor)
};
