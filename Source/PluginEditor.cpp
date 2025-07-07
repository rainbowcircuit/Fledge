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
    addAndMakeVisible(waveformDisplay);
    
    const auto params = audioProcessor.getParameters();
    for (auto param : params){
        param->addListener(this);
    }

    addAndMakeVisible(algorithmGraphics);
    
    addAndMakeVisible(practiceSlider);
    
    setSize (800, 800);
}

FledgeAudioProcessorEditor::~FledgeAudioProcessorEditor()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params){
        param->removeListener(this);
    }
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
        opInterface[oper]->setBounds(300, oper * 125 + 50, 500, 125);
    }
//    algorithmGraphics.setBounds(300, 650, 200, 200);
    
    waveformDisplay.setBounds(20, 50, 280, 500);

}
