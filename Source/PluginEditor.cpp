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

    addAndMakeVisible(showWaveformButton);
    showWaveformButton.addListener(this);
    showWaveformButton.setLookAndFeel(&showWaveLAF);
    showWaveformButton.setRadioGroupId(0);
    showWaveformButton.setToggleable(true);
    showWaveformButton.setClickingTogglesState(true);
    
    addAndMakeVisible(showAlgorithmButton);
    showAlgorithmButton.addListener(this);
    showAlgorithmButton.setLookAndFeel(&showAlgoLAF);
    showAlgorithmButton.setRadioGroupId(0);
    showAlgorithmButton.setToggleable(true);
    showAlgorithmButton.setClickingTogglesState(true);

    addAndMakeVisible(waveformDisplay);
    waveformDisplay.setVisible(true);

    algorithmGraphics = std::make_unique<AlgorithmGraphics>(audioProcessor);
    addAndMakeVisible(*algorithmGraphics);
        
    algorithmSelector = std::make_unique<AlgorithmSelectInterface>(audioProcessor, *algorithmGraphics);
    addAndMakeVisible(*algorithmSelector);
    
    algorithmGraphics->setVisible(false);
    algorithmSelector->setVisible(false);

    outputLevelMeter = std::make_unique<LevelMeter>
    (
        audioProcessor,
        audioProcessor.getOutputLevelL(),
        audioProcessor.getOutputLevelR()
    );
    addAndMakeVisible(*outputLevelMeter);

    setSize (850, 550);
    startTimerHz(30);

}

FledgeAudioProcessorEditor::~FledgeAudioProcessorEditor()
{
    showWaveformButton.removeListener(this);
    showAlgorithmButton.removeListener(this);
}

//==============================================================================
void FledgeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setColour(juce::Colour(35, 37, 36));
    g.fillAll();
    
    mainPanelBounds = { 5, 10, 320, 540 };
    mainPanelBounds.reduce(5, 5);
    juce::Path mainPanelPath;

    float tabWidth = mainPanelBounds.getWidth() * 0.125f;
    float tabHeight = mainPanelBounds.getHeight() / 2.0f;

    float tab0Width = (tabSelection == 0) ? 0.0f : tabWidth;
    float tab1Width = (tabSelection == 1) ? 0.0f : tabWidth;
    
    
    mainPanelPath.startNewSubPath(mainPanelBounds.getTopRight());
    mainPanelPath.lineTo(mainPanelBounds.getBottomRight());
    
    // tab 2
    mainPanelPath.lineTo(mainPanelBounds.getBottomLeft().x + tabWidth,
                         mainPanelBounds.getTopLeft().y + tabHeight * 2);

    mainPanelPath.lineTo(mainPanelBounds.getBottomLeft().x + tab1Width,
                         mainPanelBounds.getTopLeft().y + tabHeight * 2);
    
    mainPanelPath.lineTo(mainPanelBounds.getBottomLeft().x + tab1Width,
                         mainPanelBounds.getTopLeft().y + tabHeight);

    // tab 1 & 2
    mainPanelPath.lineTo(mainPanelBounds.getBottomLeft().x + tabWidth,
                         mainPanelBounds.getTopLeft().y + tabHeight);
    
    // tab 1
    mainPanelPath.lineTo(mainPanelBounds.getBottomLeft().x + tab0Width,
                         mainPanelBounds.getTopLeft().y + tabHeight);

    mainPanelPath.lineTo(mainPanelBounds.getBottomLeft().x + tab0Width,
                         mainPanelBounds.getTopLeft().y);
    
    mainPanelPath.lineTo(mainPanelBounds.getBottomLeft().x + tabWidth,
                         mainPanelBounds.getTopLeft().y);


    mainPanelPath.closeSubPath();
    mainPanelPath = mainPanelPath.createPathWithRoundedCorners(5.0f);
    
    g.setColour(juce::Colour(40, 42, 41));
    g.fillPath(mainPanelPath);

    //fillBackgroundPanel(g, mainPanelBounds);
}

void FledgeAudioProcessorEditor::resized()
{
    for (int oper = 0; oper < 4; oper++)
    {
        opInterface[oper]->setBounds(325, oper * 125 + 50, 520, 125);
    }

    waveformDisplay.setBounds(45, 5, 280, 530);
    outputLevelMeter->setBounds(325, 17, 300, 25);
    
    algorithmGraphics->setBounds(50, 5, 270, 355);
    algorithmSelector->setBounds(40, 345, 280, 170);
    retrieveAlgorithmGraphics();
    
    float mainPanelHeight = 540;
    float tabYIncr = mainPanelHeight/2.0f;
    float tabIconWidth = 40;
    showWaveformButton.setBounds(10,
                                 5,
                                 tabIconWidth,
                                 tabYIncr);
    
    showAlgorithmButton.setBounds(10,
                                  5 + tabYIncr,
                                  tabIconWidth,
                                  tabYIncr);
}

void FledgeAudioProcessorEditor::retrieveAlgorithmGraphics()
{
    float algorithmGraphicsWidth = 270;
    float algorithmGraphicsHeight = 355;
   // refreshing block position, probably should be a different function
        
    float op0XPos = audioProcessor.params->apvts.state.getProperty("op0XPos", algorithmGraphicsWidth/2);
    float op0YPos = audioProcessor.params->apvts.state.getProperty("op0YPos", algorithmGraphicsHeight * 0.8f);
    float op1XPos = audioProcessor.params->apvts.state.getProperty("op1XPos", algorithmGraphicsWidth/2);
    float op1YPos = audioProcessor.params->apvts.state.getProperty("op1YPos", algorithmGraphicsHeight * 0.575f);

    float op2XPos = audioProcessor.params->apvts.state.getProperty("op2XPos", algorithmGraphicsWidth/2);
    float op2YPos = audioProcessor.params->apvts.state.getProperty("op2YPos", algorithmGraphicsHeight * 0.35f);

    float op3XPos = audioProcessor.params->apvts.state.getProperty("op3XPos", algorithmGraphicsWidth/2);
    float op3YPos = audioProcessor.params->apvts.state.getProperty("op3YPos", algorithmGraphicsHeight * 0.125f);
    DBG("y upon being retrieved" << op0YPos);
    // don't write positions back into the state while restoring them
    algorithmGraphics->moveBlock(0, { op0XPos, op0YPos }, false);
    algorithmGraphics->moveBlock(1, { op1XPos, op1YPos }, false);
    algorithmGraphics->moveBlock(2, { op2XPos, op2YPos }, false);
    algorithmGraphics->moveBlock(3, { op3XPos, op3YPos }, false);
    algorithmGraphics->setGUIFromParameter();
    syncRoutingSnapshot();
}

void FledgeAudioProcessorEditor::syncRoutingSnapshot()
{
    for (int oper = 0; oper < 4; oper++)
        appliedRouting[oper] = audioProcessor.params->routing[oper]->get();
    appliedRouting[4] = audioProcessor.params->outputRouting->get();
}

bool FledgeAudioProcessorEditor::routingChangedExternally()
{
    for (int oper = 0; oper < 4; oper++)
        if (appliedRouting[oper] != (int) audioProcessor.params->routing[oper]->get())
            return true;
    return appliedRouting[4] != (int) audioProcessor.params->outputRouting->get();
}


void FledgeAudioProcessorEditor::timerCallback()
{
    // state/preset loaded (or automation) while the editor is open: rebuild the cables
    if (routingChangedExternally() && !algorithmGraphics->isInteracting())
    {
        algorithmGraphics->setGUIFromParameter();
        syncRoutingSnapshot();
    }

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
                                       audioProcessor.params->detune[oper]->getSafe());
                
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
        tabSelection = 0;
    } else if (buttonClicked == &showAlgorithmButton) {
        waveformDisplay.setVisible(false);
        algorithmGraphics->setVisible(true);
        algorithmSelector->setVisible(true);
        tabSelection = 1;

    }
    showWaveformButton.repaint();
    showAlgorithmButton.repaint();
    repaint();
}

