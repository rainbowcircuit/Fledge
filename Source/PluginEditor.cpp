/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FledgeAudioProcessorEditor::FledgeAudioProcessorEditor (FledgeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    for (int oper = 0; oper < 4; oper++)
    {
        opInterface[oper] = std::make_unique<OperatorInterface>(audioProcessor, oper);
        addAndMakeVisible(*opInterface[oper]);
    }
    presetInterface = std::make_unique<PresetInterface>(audioProcessor, audioProcessor.apvts);
    addAndMakeVisible(*presetInterface);

    addAndMakeVisible(showWaveformButton);
    showWaveformButton.addListener(this);
    addAndMakeVisible(showAlgorithmButton);
    showAlgorithmButton.addListener(this);
    
    addAndMakeVisible(waveformDisplay);
    
    const auto params = audioProcessor.getParameters();
    for (auto param : params){
        param->addListener(this);
    }

    addAndMakeVisible(algorithmGraphics);
    addAndMakeVisible(algorithmSelector);

    addAndMakeVisible(practiceSlider);
    
    setSize (800, 800);
}

FledgeAudioProcessorEditor::~FledgeAudioProcessorEditor()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params){
        param->removeListener(this);
    }
    showWaveformButton.removeListener(this);
    showAlgorithmButton.removeListener(this);

    
}

//==============================================================================
void FledgeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour(60, 62, 61));

}

void FledgeAudioProcessorEditor::resized()
{
    for (int oper = 0; oper < 4; oper++)
    {
        opInterface[oper]->setBounds(300, oper * 125 + 70, 500, 125);
    }

    presetInterface->setBounds(20, 10, 800, 50);
    waveformDisplay.setBounds(20, 70, 280, 500);
    algorithmGraphics.setBounds(20, 70, 280, 330);
    algorithmSelector.setBounds(20, 390, 280, 150);
    
    showWaveformButton.setBounds(20, 570, 140, 40);
    showAlgorithmButton.setBounds(160, 570, 140, 40);

}
