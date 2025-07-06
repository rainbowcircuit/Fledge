/*
  ==============================================================================

    UserInterface.cpp
    Created: 30 May 2025 11:28:23am
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "UserInterface.h"

OperatorInterface::OperatorInterface(FledgeAudioProcessor& p, int index) : audioProcessor(p)
{
    this->index = index;
    
    setSlider(ratioSlider, ratioLabel, "Ratio");
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ratio" + juce::String(index), ratioSlider);

    setSlider(modIndexSlider, modIndexLabel, "Mod Index");
    modIndexAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "modIndex" + juce::String(index), modIndexSlider);

    setSlider(attackSlider, attackLabel, "Attack");
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "attack" + juce::String(index), attackSlider);

    setSlider(decaySlider, decayLabel, "Decay");
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "decay" + juce::String(index), decaySlider);

    setSlider(sustainSlider, sustainLabel, "Sustain");
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sustain" + juce::String(index), sustainSlider);

    setSlider(releaseSlider, releaseLabel, "Release");
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "release" + juce::String(index), releaseSlider);

    addAndMakeVisible(envGraphics);
    addAndMakeVisible(opGraphics);
    startTimerHz(30);
}



void OperatorInterface::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    bounds.reduce(5, 5);
    
    juce::Path boundsPath;
    boundsPath.addRoundedRectangle(bounds, 5, 5);
    
    g.setColour(juce::Colour(36, 159, 208));
    g.fillPath(boundsPath);
}

void OperatorInterface::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    float sliderSize = width * 0.15f;
    float labelHeight = height * 0.15f;
    
    
    
    ratioLabel.setBounds(x, y, sliderSize, labelHeight);
    modIndexLabel.setBounds(x + sliderSize, y, sliderSize, labelHeight);
    /*
    attackLabel.setBounds(x + sliderSize * 2, y, sliderSize, labelHeight);
    decayLabel.setBounds(x + sliderSize * 3, y, sliderSize, labelHeight);
    sustainLabel.setBounds(x + sliderSize * 4, y, sliderSize, labelHeight);
    releaseLabel.setBounds(x + sliderSize * 5, y, sliderSize, labelHeight);
*/
    
    ratioSlider.setBounds(x + 20, y + labelHeight, sliderSize, sliderSize/2);
    modIndexSlider.setBounds(x + 20, y + labelHeight + sliderSize/2, sliderSize, sliderSize/2);
    /*
    attackSlider.setBounds(x + sliderSize * 2, y + labelHeight, sliderSize, sliderSize);
    decaySlider.setBounds(x + sliderSize * 3, y + labelHeight, sliderSize, sliderSize);
    sustainSlider.setBounds(x + sliderSize * 4, y + labelHeight, sliderSize, sliderSize);
    releaseSlider.setBounds(x + sliderSize * 5, y + labelHeight, sliderSize, sliderSize);
    */
    
    envGraphics.setBounds(x + sliderSize * 4 + 20, y + labelHeight, sliderSize * 2, sliderSize * 1.5f);
    opGraphics.setBounds(x + sliderSize * 2, y + labelHeight, sliderSize * 2, sliderSize * 1.5f);
}

void OperatorInterface::setSlider(juce::Slider &s, juce::Label &l, juce::String labelText)
{
    addAndMakeVisible(s);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 50.0f, 10.0f); // for now
    s.setLookAndFeel(&dialLAF);
    
    addAndMakeVisible(l);
    l.setText(labelText, juce::NotificationType::dontSendNotification);
}


void OperatorInterface::setIndex(int index)
{
    this->index = index;
}

void OperatorInterface::timerCallback()
{
    float ratio = audioProcessor.apvts.getRawParameterValue("ratio" + juce::String(index))->load();
    float fixed = audioProcessor.apvts.getRawParameterValue("fixed" + juce::String(index))->load();
    float modIndex = audioProcessor.apvts.getRawParameterValue("modIndex" + juce::String(index))->load();
    bool opMode = audioProcessor.apvts.getRawParameterValue("opMode" + juce::String(index))->load();
    opGraphics.setRatioAndAmplitude(ratio, fixed, modIndex, opMode);
    
    float attack = audioProcessor.apvts.getRawParameterValue("attack" + juce::String(index))->load();
    float decay = audioProcessor.apvts.getRawParameterValue("release" + juce::String(index))->load();
    float sustain = audioProcessor.apvts.getRawParameterValue("sustain" + juce::String(index))->load();
    bool release = audioProcessor.apvts.getRawParameterValue("release" + juce::String(index))->load();
    envGraphics.setEnvelope(attack, decay, sustain, release);

}
