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
    showWaveformButton.setLookAndFeel(&showWaveLAF);
    
    addAndMakeVisible(showAlgorithmButton);
    showAlgorithmButton.addListener(this);
    showAlgorithmButton.setLookAndFeel(&showAlgoLAF);

    addAndMakeVisible(showMacrosButton);
    showMacrosButton.addListener(this);
    showMacrosButton.setLookAndFeel(&showMacroLAF);

    
    addAndMakeVisible(waveformDisplay);
    waveformDisplay.setVisible(true);

    addAndMakeVisible(algorithmGraphics);
    algorithmSelector = std::make_unique<AlgorithmSelectInterface>(audioProcessor, algorithmGraphics);
    addAndMakeVisible(*algorithmSelector);
    
    macroInterface = std::make_unique<MacroControlsInterface>(audioProcessor);
    addAndMakeVisible(*macroInterface);
    
    algorithmGraphics.setVisible(false);
    algorithmSelector->setVisible(false);
    macroInterface->setVisible(false);

    const auto params = audioProcessor.getParameters();
    for (auto param : params){
        param->addListener(this);
    }
    setSize (765, 600);
}

FledgeAudioProcessorEditor::~FledgeAudioProcessorEditor()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params){
        param->removeListener(this);
    }
    showWaveformButton.removeListener(this);
    showAlgorithmButton.removeListener(this);
    showMacrosButton.removeListener(this);

}

//==============================================================================
void FledgeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setColour(juce::Colour(35, 37, 36));
    g.fillAll();
    
    juce::Path leftBounds;
    leftBounds.addRoundedRectangle(5, 60, 280, 490, 5.0f);
    g.setColour(juce::Colour(40, 42, 41));
    g.fillPath(leftBounds);
}

void FledgeAudioProcessorEditor::resized()
{
    for (int oper = 0; oper < 4; oper++)
    {
        opInterface[oper]->setBounds(285, oper * 125 + 55, 480, 125);
    }

    presetInterface->setBounds(5, 5, 760, 50);
    waveformDisplay.setBounds(5, 60, 280, 490);
    algorithmGraphics.setBounds(5, 60, 280, 350);
    algorithmSelector->setBounds(5, 400, 280, 150);
    macroInterface->setBounds(5, 60, 280, 490);

    showWaveformButton.setBounds(5, 555, 94, 40);
    showAlgorithmButton.setBounds(100, 555, 94, 40);
    showMacrosButton.setBounds(195, 555, 94, 40);
}
