/*
  ==============================================================================

    OperatorLayout.cpp
    Created: 8 Aug 2025 2:31:50pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "OperatorLayout.h"
OperatorInterface::OperatorInterface(FledgeAudioProcessor& p, int index) : audioProcessor(p)
{
    this->index = index;
    
    setLabel(ratioLabel, "Ratio", 12.0f);
    ratioSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "ratio" + juce::String(index), ":1");
    addAndMakeVisible(*ratioSlider);
    ratioSlider->setFontSize(12.0f);
    ratioSlider->setNumDecimals(2);
    
    setLabel(fixedLabel, "Fixed", 12.0f);
    fixedSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "fixed" + juce::String(index), "");
    addAndMakeVisible(*fixedSlider);
    fixedSlider->setFontSize(24.0f);

    setLabel(amplitudeLabel, "Amp", 12.0f);
    amplitudeSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "amplitude" + juce::String(index), "%");
    addAndMakeVisible(*amplitudeSlider);
    amplitudeSlider->setFontSize(12.0f);

    setLabel(phaseLabel, "P", 12.0f);
    phaseSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "phase" + juce::String(index), "%");
    addAndMakeVisible(*phaseSlider);
    phaseSlider->setFontSize(12.0f);

    setLabel(attackLabel, "A", 12.0f);
    attackSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "attack" + juce::String(index), " ms");
    addAndMakeVisible(*attackSlider);
    attackSlider->setFontSize(12.0f);
    
    setLabel(decayLabel, "D", 12.0f);
    decaySlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "decay" + juce::String(index), " ms");
    addAndMakeVisible(*decaySlider);
    decaySlider->setFontSize(12.0f);

    setLabel(sustainLabel, "S", 12.0f);
    sustainSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "sustain" + juce::String(index), " %");
    addAndMakeVisible(*sustainSlider);
    sustainSlider->setFontSize(12.0f);

    setLabel(releaseLabel, "R", 12.0f);
    releaseSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "release" + juce::String(index), " ms");
    addAndMakeVisible(*releaseSlider);
    releaseSlider->setFontSize(12.0f);

    envGraphics = std::make_unique<EnvelopeDisplayGraphics>(audioProcessor, index);
    addAndMakeVisible(*envGraphics);
    
    opGraphics = std::make_unique<OperatorDisplayGraphics>(audioProcessor);
    addAndMakeVisible(*opGraphics);
    opGraphics->setIndex(index);

    startTimerHz(30);
}

void OperatorInterface::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    fillBackgroundPanel(g, bounds);
    bounds.reduce(5, 5);
    

}

void OperatorInterface::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    amplitudeLabel.setBounds(x + width * 0.0125f, height * 0.025f, width * 0.15f, height * 0.2f);
    amplitudeSlider->setBounds(x + width * 0.075, height * 0.025f, width * 0.15f, height * 0.2f);

    ratioLabel.setBounds(x + width * 0.175f, height * 0.025f, width * 0.15f, height * 0.2f);
    ratioSlider->setBounds(x + width * 0.245f, height * 0.025f, width * 0.175f, height * 0.2f);

    opGraphics->setBounds(x + width * 0.0125f,
                          y + height * 0.2f,
                          width * 0.45f,
                          height * 0.7625f);

    phaseLabel.setBounds(x + width * 0.325f,  height * 0.025f, width * 0.15f, height * 0.2f);
    phaseSlider->setBounds(x + width * 0.35f, height * 0.025f, width * 0.15f, height * 0.2f);
    
    attackLabel.setBounds(x + width * 0.825f,
                          y + height * 0.1f,
                          width * 0.05f,
                          height * 0.2f);
    
    decayLabel.setBounds(x + width * 0.825f,
                         y + height * 0.3f,
                         width * 0.05f,
                         height * 0.2f);
    
    sustainLabel.setBounds(x + width * 0.825f,
                           y + height * 0.5f,
                           width * 0.05f,
                           height * 0.2f);
    
    releaseLabel.setBounds(x + width * 0.825f,
                           y + height * 0.7f,
                           width * 0.05f,
                           height * 0.2f);
    
    attackSlider->setBounds(x + width * 0.865f,
                            y + height * 0.1f,
                            width * 0.165f,
                            height * 0.2f);
    decaySlider->setBounds(x + width * 0.865f,
                           y + height * 0.3f,
                           width * 0.165f,
                           height * 0.2f);
    sustainSlider->setBounds(x + width * 0.865f, y + height * 0.5f, width * 0.165f, height * 0.2f);
    releaseSlider->setBounds(x + width * 0.865f, y + height * 0.7f, width * 0.165f, height * 0.2f);

    envGraphics->setBounds(x + width * 0.4525f, y + height * 0.05f, width * 0.375f, height * 0.9125f);
}

void OperatorInterface::setLabel(juce::Label &l, juce::String labelText, float size)
{
    addAndMakeVisible(l);
    l.setText(labelText, juce::NotificationType::dontSendNotification);
    l.setFont(juce::FontOptions(size, juce::Font::plain));
    l.setColour(juce::Label::textColourId, juce::Colour(150, 150, 150));
}

void OperatorInterface::setIndex(int index)
{
    this->index = index;
}

void OperatorInterface::timerCallback()
{
    float ratio = audioProcessor.params->apvts.getRawParameterValue("ratio" + juce::String(index))->load();
    float fixed = audioProcessor.params->apvts.getRawParameterValue("fixed" + juce::String(index))->load();
    float modIndex = audioProcessor.params->apvts.getRawParameterValue("amplitude" + juce::String(index))->load();
    bool opMode = audioProcessor.params->apvts.getRawParameterValue("opMode" + juce::String(index))->load();
    float phase = audioProcessor.params->apvts.getRawParameterValue("phase" + juce::String(index))->load();

    opGraphics->setRatioAndAmplitude(ratio, fixed, modIndex, opMode, phase);
    
    float attack = audioProcessor.params->apvts.getRawParameterValue("attack" + juce::String(index))->load();
    float decay = audioProcessor.params->apvts.getRawParameterValue("decay" + juce::String(index))->load();
    float sustain = audioProcessor.params->apvts.getRawParameterValue("sustain" + juce::String(index))->load();
    float release = audioProcessor.params->apvts.getRawParameterValue("release" + juce::String(index))->load();
    
    float globalAttack = audioProcessor.params->apvts.getRawParameterValue("globalAttack")->load();
    float globalDecay = audioProcessor.params->apvts.getRawParameterValue("globalDecay")->load();
    float globalSustain = audioProcessor.params->apvts.getRawParameterValue("globalSustain")->load();
    float globalRelease = audioProcessor.params->apvts.getRawParameterValue("globalRelease")->load();

    envGraphics->setEnvelope(attack, decay, sustain, release, globalAttack, globalDecay, globalSustain, globalRelease);

}
