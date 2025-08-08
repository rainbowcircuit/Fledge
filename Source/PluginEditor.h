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
class FledgeAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::AudioProcessorParameter::Listener, juce::Button::Listener, GraphicsHelper
{
public:
    FledgeAudioProcessorEditor (FledgeAudioProcessor&);
    ~FledgeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void initializeParameter()
    {
        for (int oper = 0; oper < 4; oper++)
        {
            juce::String attackID = "attack" + juce::String(oper);
            juce::String decayID = "decay" + juce::String(oper);
            juce::String sustainID = "sustain" + juce::String(oper);
            juce::String releaseID = "release" + juce::String(oper);

            float attack = audioProcessor.params->apvts.getRawParameterValue(attackID)->load();
            float decay = audioProcessor.params->apvts.getRawParameterValue(decayID)->load();
            float sustain = audioProcessor.params->apvts.getRawParameterValue(sustainID)->load();
            float release = audioProcessor.params->apvts.getRawParameterValue(releaseID)->load();
            
            waveformDisplay.setEnvelope(oper, attack, decay, sustain, release);

            juce::String ratioID = "ratio" + juce::String(oper);
            juce::String amplitudeID = "amplitude" + juce::String(oper);
            juce::String phaseID = "phase" + juce::String(oper);

            float ratio = audioProcessor.params->apvts.getRawParameterValue(ratioID)->load();
            float amplitude = audioProcessor.params->apvts.getRawParameterValue(amplitudeID)->load();
            float phase = audioProcessor.params->apvts.getRawParameterValue(phaseID)->load();

            juce::String gainIndexID = "operator" + juce::String(oper) + "Routing";
            float gainIndex = audioProcessor.params->apvts.getRawParameterValue(gainIndexID)->load();
            float outputGainIndex = audioProcessor.params->apvts.getRawParameterValue("outputRouting")->load();

            waveformDisplay.setFMParameter(oper, ratio, 0.0f, true, amplitude, phase);
            waveformDisplay.setGainCoefficients(oper, gainIndex, outputGainIndex);
        }
    }
    
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        
        float outputGainIndex = audioProcessor.params->apvts.getRawParameterValue("outputRouting")->load();
        algorithmGraphics->op[4].setInput(outputGainIndex);

        for (int oper = 0; oper < 4; oper++)
        {
            juce::String attackID = "attack" + juce::String(oper);
            juce::String decayID = "decay" + juce::String(oper);
            juce::String sustainID = "sustain" + juce::String(oper);
            juce::String releaseID = "release" + juce::String(oper);

            float attack = audioProcessor.params->apvts.getRawParameterValue(attackID)->load();
            float decay = audioProcessor.params->apvts.getRawParameterValue(decayID)->load();
            float sustain = audioProcessor.params->apvts.getRawParameterValue(sustainID)->load();
            float release = audioProcessor.params->apvts.getRawParameterValue(releaseID)->load();
            
            waveformDisplay.setEnvelope(oper, attack, decay, sustain, release);

            juce::String ratioID = "ratio" + juce::String(oper);
            juce::String amplitudeID = "amplitude" + juce::String(oper);
            juce::String phaseID = "phase" + juce::String(oper);

            float ratio = audioProcessor.params->apvts.getRawParameterValue(ratioID)->load();
            float amplitude = audioProcessor.params->apvts.getRawParameterValue(amplitudeID)->load();
            float phase = audioProcessor.params->apvts.getRawParameterValue(phaseID)->load();

            juce::String gainIndexID = "operator" + juce::String(oper) + "Routing";
            float gainIndex = audioProcessor.params->apvts.getRawParameterValue(gainIndexID)->load();
            
            algorithmGraphics->op[oper].setInput(gainIndex);

            waveformDisplay.setFMParameter(oper, ratio, 0.0f, true, amplitude, phase);
            waveformDisplay.setGainCoefficients(oper, gainIndex, outputGainIndex);

        }
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
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
        editorState.setProperty("width", 0.5f, nullptr);
        editorState.setProperty("height", 0.5f, nullptr);
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
    juce::ValueTree editorState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessorEditor)
};
