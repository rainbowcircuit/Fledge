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

    //
    addAndMakeVisible(practiceSlider);
    practiceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    practiceSlider.setLookAndFeel(&practiceLookAndFeel);
    practiceSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    
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
    g.fillAll(juce::Colour(235, 235, 235));

}

void FledgeAudioProcessorEditor::resized()
{
    for (int oper = 0; oper < 4; oper++)
    {
        opInterface[oper]->setBounds(300, oper * 150 + 100, 500, 150);
    }
    practiceSlider.setBounds(300, 650, 100, 100);
    
    waveformDisplay.setBounds(20, 100, 280, 600);

}
