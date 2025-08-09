/*
  ==============================================================================

    UserInterface.cpp
    Created: 30 May 2025 11:28:23am
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "UserInterface.h"


MacroControlsInterface::MacroControlsInterface(FledgeAudioProcessor& p) : audioProcessor(p)
{
    setSliderAndLabel(globalModIndexSlider, globalModIndexLabel, dialLAF, "Mod Scale", "%");
    globalModIndexAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "globalModIndex", globalModIndexSlider);

    setSliderAndLabel(globalAttackSlider, globalAttackLabel, dialLAF, "Attack Scale", "%");
    globalAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "globalAttack", globalAttackSlider);
    
    setSliderAndLabel(globalDecaySlider, globalDecayLabel, dialLAF, "Decay Scale", "%");
    globalDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "globalDecay", globalDecaySlider);
    
    setSliderAndLabel(globalSustainSlider, globalSustainLabel, dialLAF, "Sustain Scale", "%");
    globalSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "globalSustain", globalSustainSlider);

    setSliderAndLabel(globalReleaseSlider, globalReleaseLabel, dialLAF, "Release Scale", "%");
    globalReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "globalRelease", globalReleaseSlider);

}

void MacroControlsInterface::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    float sliderSize = width * 0.25f;
    
    globalModIndexLabel.setBounds(x + (width/2) - (sliderSize/2),
                                   y + height * 0.05f,
                                  sliderSize,
                                  sliderSize/4);

    globalModIndexSlider.setBounds(x + (width/2) - (sliderSize/2),
                                   y + height * 0.1f,
                                   sliderSize,
                                   sliderSize * 1.15f);
    
    // attack
    globalAttackLabel.setBounds(x + width * 0.125f,
                                y + height * 0.325f,
                                sliderSize,
                                sliderSize/4);

    globalAttackSlider.setBounds(x + width * 0.125f,
                                 y + height * 0.375f,
                                 sliderSize,
                                 sliderSize * 1.15f);
        
    // decay
    globalDecayLabel.setBounds(x + width * 0.625f,
                               y + height * 0.325f,
                               sliderSize,
                               sliderSize/4);
    
    globalDecaySlider.setBounds(x + width * 0.625f,
                                y + height * 0.375f,
                                sliderSize,
                                sliderSize * 1.15f);

    // sustain
    globalSustainLabel.setBounds(x + width * 0.125f,
                                 y + height * 0.6f,
                                 sliderSize,
                                 sliderSize/4);
    
    globalSustainSlider.setBounds(x + width * 0.125f,
                                  y + height * 0.65f,
                                  sliderSize,
                                  sliderSize * 1.15f);

    // release
    globalReleaseLabel.setBounds(x + width * 0.625f,
                                 y + height * 0.6f,
                                 sliderSize,
                                 sliderSize/4);
    
    globalReleaseSlider.setBounds(x + width * 0.625f,
                                  y + height * 0.65f,
                                  sliderSize,
                                  sliderSize * 1.15f);


}
