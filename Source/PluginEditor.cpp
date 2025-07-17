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
    showWaveformButton.setLookAndFeel(&showWaveformLAF);

    addAndMakeVisible(showAlgorithmButton);
    showAlgorithmButton.setLookAndFeel(&showAlgorithmLAF);
    showAlgorithmButton.addListener(this);
    
    addAndMakeVisible(waveformDisplay);
    
    const auto params = audioProcessor.getParameters();
    for (auto param : params){
        param->addListener(this);
    }

    addAndMakeVisible(algorithmGraphics);
    addAndMakeVisible(algorithmSelector);

    addAndMakeVisible(practiceSlider);
    
    setSize (800, 580);
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
    
    juce::Path leftBoundsFill;
    g.setColour(juce::Colour(40, 42, 41));
    leftBoundsFill.addRoundedRectangle(10, 70, 280, 460, 10.0f);
    g.fillPath(leftBoundsFill);
}

void FledgeAudioProcessorEditor::resized()
{
    for (int oper = 0; oper < 4; oper++)
    {
        opInterface[oper]->setBounds(290, oper * 125 + 70, 500, 125);
    }

    presetInterface->setBounds(10, 10, 800, 50);
    waveformDisplay.setBounds(10, 70, 280, 460);
    algorithmGraphics.setBounds(10, 70, 280, 330);
    algorithmSelector.setBounds(10, 390, 280, 150);
    
    showWaveformButton.setBounds(10, 530, 140, 40);
    showAlgorithmButton.setBounds(150, 530, 140, 40);

}
