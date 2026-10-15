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

    void retrieveAlgorithmGraphics();
    void syncRoutingSnapshot();
    bool routingChangedExternally();


private:
    int tabSelection = 0;
    std::array<int, 5> appliedRouting{};
    juce::Rectangle<float> mainPanelBounds;
    
    ButtonLookAndFeel showWaveLAF { 3 }, showAlgoLAF { 4 };
    
    std::array<std::unique_ptr<OperatorInterface>, 4>  opInterface;
    
    juce::TextButton showWaveformButton, showAlgorithmButton;
    WaveformDisplayGraphics waveformDisplay;
    std::unique_ptr<AlgorithmGraphics> algorithmGraphics;
    std::unique_ptr<AlgorithmSelectInterface> algorithmSelector;
    
    std::unique_ptr<LevelMeter> outputLevelMeter;

    FledgeAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessorEditor)
};
