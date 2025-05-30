/*
  ==============================================================================

    UserInterface.cpp
    Created: 30 May 2025 11:28:23am
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "UserInterface.h"
OperatorInterface::OperatorInterface()
{
    setSlider(ratioSlider, ratioLabel, "Ratio");
    setSlider(modIndexSlider, modIndexLabel, "Mod Index");
    setSlider(attackSlider, attackLabel, "Attack");
    setSlider(decaySlider, decayLabel, "Decay");
    setSlider(sustainSlider, sustainLabel, "Sustain");
    setSlider(releaseSlider, releaseLabel, "Release");
    
    addAndMakeVisible(operGraphic);
}

void OperatorInterface::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(120, 120, 120));
    g.fillRoundedRectangle(bounds, 10);
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
    
    operGraphic.setBounds(x + 10, y + 10, sliderSize * 1.8f, sliderSize);
   // ratioLabel.setBounds(x, y, sliderSize, labelHeight);
   // modIndexLabel.setBounds(x + sliderSize, y, sliderSize, labelHeight);
    attackLabel.setBounds(x + sliderSize * 2, y, sliderSize, labelHeight);
    decayLabel.setBounds(x + sliderSize * 3, y, sliderSize, labelHeight);
    sustainLabel.setBounds(x + sliderSize * 4, y, sliderSize, labelHeight);
    releaseLabel.setBounds(x + sliderSize * 5, y, sliderSize, labelHeight);

    //ratioSlider.setBounds(x, y + labelHeight, sliderSize, sliderSize);
   // modIndexSlider.setBounds(x + sliderSize, y + labelHeight, sliderSize, sliderSize);
    attackSlider.setBounds(x + sliderSize * 2, y + labelHeight, sliderSize, sliderSize);
    decaySlider.setBounds(x + sliderSize * 3, y + labelHeight, sliderSize, sliderSize);
    sustainSlider.setBounds(x + sliderSize * 4, y + labelHeight, sliderSize, sliderSize);
    releaseSlider.setBounds(x + sliderSize * 5, y + labelHeight, sliderSize, sliderSize);

}

void OperatorInterface::setSlider(juce::Slider &s, juce::Label &l, juce::String labelText)
{
    
    addAndMakeVisible(s);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50.0f, 10.0f); // for now
  // slider.setLookAndFeel(nullptr);
    
    addAndMakeVisible(l);
    l.setText(labelText, juce::NotificationType::dontSendNotification);
}

void OperatorInterface::setIndex(int index)
{
    this->index = index;
}
