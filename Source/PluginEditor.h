/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UserInterface.h"
#include "AlgorithmGraphics.h"
#include "ButtonLookAndFeel.h"
#include "LookAndFeel.h"
#include "LevelMeter.h"
#include "GraphicsUtility.h"

//==============================================================================
/**
*/
class FledgeAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer, juce::Button::Listener, GraphicsHelper
{
public:
    FledgeAudioProcessorEditor (FledgeAudioProcessor&);
    ~FledgeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void buttonClicked(juce::Button* buttonClicked) override
    {
        if (buttonClicked == &showWaveformButton)
        {
            waveformDisplay.setVisible(true);
            algorithmGraphics->setVisible(false);
            algorithmSelector->setVisible(false);
            macroInterface->setVisible(false);

        } else if (buttonClicked == &showAlgorithmButton) {
            waveformDisplay.setVisible(false);
            algorithmGraphics->setVisible(true);
            algorithmSelector->setVisible(true);
            macroInterface->setVisible(false);

        } else if (buttonClicked == &showMacrosButton) {
            waveformDisplay.setVisible(false);
            algorithmGraphics->setVisible(false);
            algorithmSelector->setVisible(false);
            macroInterface->setVisible(true);
        }
    }

    
    
    void initializeEditorState()
    {
   //     editorState.setProperty("width", 0.5f, nullptr);
    //    editorState.setProperty("height", 0.5f, nullptr);
    }


private:
    ButtonLookAndFeel showWaveLAF { 3 }, showAlgoLAF { 4 }, showMacroLAF { 5 };
    
    std::array<std::unique_ptr<OperatorInterface>, 4>  opInterface;
    std::unique_ptr<PresetInterface>  presetInterface;
    std::unique_ptr<MacroControlsInterface> macroInterface;
    
    juce::TextButton showWaveformButton, showAlgorithmButton, showMacrosButton;
    WaveformDisplayGraphics waveformDisplay;
    std::unique_ptr<AlgorithmGraphics> algorithmGraphics;
    std::unique_ptr<AlgorithmSelectInterface> algorithmSelector;
    
    std::unique_ptr<LevelMeter> outputLevelMeter;

    FledgeAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessorEditor)
};
