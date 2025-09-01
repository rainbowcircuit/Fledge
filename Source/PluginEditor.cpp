/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "PluginEditor.h"
#include "AlgorithmSelector.h"
#include "PluginProcessor.h"

//==============================================================================
FledgeAudioProcessorEditor::FledgeAudioProcessorEditor (FledgeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    for (int oper = 0; oper < 4; oper++)
    {
        opInterface[oper] = std::make_unique<OperatorInterface>(audioProcessor, oper);
        addAndMakeVisible(*opInterface[oper]);
    }
    presetInterface = std::make_unique<PresetInterface>(audioProcessor, audioProcessor.params->apvts);
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

    algorithmGraphics = std::make_unique<AlgorithmGraphics>(audioProcessor);
    addAndMakeVisible(*algorithmGraphics);
        
    algorithmSelector = std::make_unique<AlgorithmSelectInterface>(audioProcessor, *algorithmGraphics);
    addAndMakeVisible(*algorithmSelector);
    
    macroInterface = std::make_unique<MacroControlsInterface>(audioProcessor);
    addAndMakeVisible(*macroInterface);
    
    algorithmGraphics->setVisible(false);
    algorithmSelector->setVisible(false);
    macroInterface->setVisible(false);

    outputLevelMeter = std::make_unique<LevelMeter>
    (
        audioProcessor.getOutputLevelL(),
        audioProcessor.getOutputLevelR()
    );
    addAndMakeVisible(*outputLevelMeter);

    setSize (770, 605);
    startTimerHz(30);

}

FledgeAudioProcessorEditor::~FledgeAudioProcessorEditor()
{
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
    juce::Rectangle<float> mainPanelBounds = { 5, 60, 280, 490 };
    fillBackgroundPanel(g, mainPanelBounds);
}

void FledgeAudioProcessorEditor::resized()
{
    const int width = getWidth();
    const int height = getHeight();
    const float margin = width * 0.00641f;

    for (int oper = 0; oper < 4; oper++)
    {
        opInterface[oper]->setBounds(285, oper * 125 + 100, 480, 125);
    }


    presetInterface->setBounds(5, 5, 760, 40);
    waveformDisplay.setBounds(5, 60, 280, 490);
    algorithmGraphics->setBounds(10, 65, 270, 335);
    algorithmSelector->setBounds(5, 400, 280, 150);
    
   // juce::String index = juce::String(oper);
    float op0XPos = audioProcessor.params->apvts.state.getProperty("op0XPos");
    float op0YPos = audioProcessor.params->apvts.state.getProperty("op0YPos");
    
    float op1XPos = audioProcessor.params->apvts.state.getProperty("op1XPos");
    float op1YPos = audioProcessor.params->apvts.state.getProperty("op1YPos");

    float op2XPos = audioProcessor.params->apvts.state.getProperty("op2XPos");
    float op2YPos = audioProcessor.params->apvts.state.getProperty("op2YPos");

    float op3XPos = audioProcessor.params->apvts.state.getProperty("op3XPos");
    float op3YPos = audioProcessor.params->apvts.state.getProperty("op3YPos");

    algorithmGraphics->moveBlock(0, { op0XPos, op0YPos });
    algorithmGraphics->moveBlock(1, { op1XPos, op1YPos });
    algorithmGraphics->moveBlock(2, { op2XPos, op2YPos });
    algorithmGraphics->moveBlock(3, { op3XPos, op3YPos });

    
    
    
    macroInterface->setBounds(5, 60, 280, 490);

    outputLevelMeter->setBounds(285, 72, 300, 25);

    showWaveformButton.setBounds(5, 555, 94, 40);
    showAlgorithmButton.setBounds(100, 555, 94, 40);
    showMacrosButton.setBounds(195, 555, 94, 40);
}



void FledgeAudioProcessorEditor::timerCallback()
{
    for (int oper = 0; oper < 4; oper++)
    {
        waveformDisplay.setEnvelope(oper,
                                    audioProcessor.params->attack[oper]->getSafe(),
                                    audioProcessor.params->decay[oper]->getSafe(),
                                    audioProcessor.params->sustain[oper]->getSafe(),
                                    audioProcessor.params->release[oper]->getSafe());
        
        waveformDisplay.setFMParameter(oper,
                                       audioProcessor.params->ratio[oper]->getSafe(),
                                       0.0f,
                                       true,
                                       audioProcessor.params->amplitude[oper]->getSafe(),
                                       audioProcessor.params->phase[oper]->getSafe());
                
        waveformDisplay.setGainCoefficients(oper,
                                            audioProcessor.params->routing[oper]->getSafe(),
                                            audioProcessor.params->outputRouting->getSafe());

        algorithmGraphics->op[oper].setInput(audioProcessor.params->routing[oper]->getSafe());
        algorithmGraphics->op[oper].setOpacity(audioProcessor.params->amplitude[oper]->getSafe());
    }
    algorithmGraphics->op[4].setInput(audioProcessor.params->outputRouting->getSafe());
    waveformDisplay.repaint(); // paint for internal slew limiting
}

void FledgeAudioProcessorEditor::buttonClicked(juce::Button* buttonClicked)
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
