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
    
    setLabel(ratioLabel, "Ratio", 12.0f);
    ratioSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "ratio" + juce::String(index), "");
    addAndMakeVisible(*ratioSlider);
    ratioSlider->setFontSize(24.0f);

    setLabel(fixedLabel, "Fixed", 12.0f);
    fixedSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "fixed" + juce::String(index), "");
    addAndMakeVisible(*fixedSlider);
    fixedSlider->setFontSize(24.0f);

    setLabel(amplitudeLabel, "Amplitude", 12.0f);
    amplitudeSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "amplitude" + juce::String(index), "");
    addAndMakeVisible(*amplitudeSlider);
    amplitudeSlider->setFontSize(24.0f);

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

    addAndMakeVisible(envGraphics);
    addAndMakeVisible(opGraphics);
    envGraphics.setIndex(index);
    opGraphics.setIndex(index);

    startTimerHz(30);
}

void OperatorInterface::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    bounds.reduce(5, 5);
    
    juce::Path boundsPath;
    boundsPath.addRoundedRectangle(bounds, 5, 5);
    g.setColour(juce::Colour(40, 42, 41));
    g.fillPath(boundsPath);
    g.setColour(juce::Colour(35, 37, 36));
    g.strokePath(boundsPath, juce::PathStrokeType(2.0f));
    
}

void OperatorInterface::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    float sliderSize = width * 0.15f;
    float heightMargin = height * 0.1f;
    float labelHeight = height * 0.1f;
    float textSliderHeight = height * 0.25f;

    
    ratioLabel.setBounds(x + width * 0.025f, y + height * 0.1f, width * 0.15f, height * 0.1f);
    ratioSlider->setBounds(x + width * 0.025f, y + height * 0.1f + labelHeight, width * 0.15f, height * 0.25f);

    amplitudeLabel.setBounds(x + width * 0.025f,  height * 0.6f, width * 0.15f, height * 0.1f);
    amplitudeSlider->setBounds(x + width * 0.025f, height * 0.6f + labelHeight, width * 0.15f, height * 0.25f);

    opGraphics.setBounds(x + 100, y + height * 0.125f, sliderSize * 2, height * 0.75f);

    attackLabel.setBounds(x + width * 0.8f,
                          y + height * 0.1f,
                          width * 0.035f,
                          height * 0.2f);
    
    decayLabel.setBounds(x + width * 0.8f,
                         y + height * 0.3f,
                         width * 0.035f,
                         height * 0.2f);
    
    sustainLabel.setBounds(x + width * 0.8f,
                           y + height * 0.5f,
                           width * 0.035f,
                           height * 0.2f);
    
    releaseLabel.setBounds(x + width * 0.8f,
                           y + height * 0.7f,
                           width * 0.035f,
                           height * 0.2f);
    
    attackSlider->setBounds(x + width * 0.835f,  y + height * 0.1f, width * 0.165f, height * 0.2f);
    decaySlider->setBounds(x + width * 0.835f,   y + height * 0.3f, width * 0.165f, height * 0.2f);
    sustainSlider->setBounds(x + width * 0.835f, y + height * 0.5f, width * 0.165f, height * 0.2f);
    releaseSlider->setBounds(x + width * 0.835f, y + height * 0.7f, width * 0.165f, height * 0.2f);

    envGraphics.setBounds(x + sliderSize * 3.5 , y + height * 0.125f, sliderSize * 2, height * 0.75f);
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
    float ratio = audioProcessor.apvts.getRawParameterValue("ratio" + juce::String(index))->load();
    float fixed = audioProcessor.apvts.getRawParameterValue("fixed" + juce::String(index))->load();
    float modIndex = audioProcessor.apvts.getRawParameterValue("amplitude" + juce::String(index))->load();
    bool opMode = audioProcessor.apvts.getRawParameterValue("opMode" + juce::String(index))->load();
    opGraphics.setRatioAndAmplitude(ratio, fixed, modIndex, opMode);
    
    float attack = audioProcessor.apvts.getRawParameterValue("attack" + juce::String(index))->load();
    float decay = audioProcessor.apvts.getRawParameterValue("decay" + juce::String(index))->load();
    float sustain = audioProcessor.apvts.getRawParameterValue("sustain" + juce::String(index))->load();
    float release = audioProcessor.apvts.getRawParameterValue("release" + juce::String(index))->load();
    envGraphics.setEnvelope(attack, decay, sustain, release);

}


PresetInterface::PresetInterface(FledgeAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts) : presetManager(apvts), audioProcessor(p)
{
    juce::FontOptions font { 12.0f, juce::Font::plain };

    addAndMakeVisible(saveButton);
    saveButton.addListener(this);
 //   saveButton.setLookAndFeel(&saveLAF);

    addAndMakeVisible(nextButton);
    nextButton.addListener(this);
//    nextButton.setLookAndFeel(&nextLAF);

    addAndMakeVisible(prevButton);
    prevButton.addListener(this);
 //   prevButton.setLookAndFeel(&prevLAF);
    
    addAndMakeVisible(presetComboBox);
    presetComboBox.addListener(this);
 //   presetComboBox.setLookAndFeel(&comboBoxLAF);
    
    // refresh presets
    loadPresetList();
}

PresetInterface::~PresetInterface()
{
    saveButton.removeListener(this);
    nextButton.removeListener(this);
    prevButton.removeListener(this);
    presetComboBox.removeListener(this);
}

void PresetInterface::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float height = bounds.getHeight();

    rateLabel.setBounds(600, y + height * 0.25f, 164, height * 0.35f);
    rateValueLabel.setBounds(605, y + height * 0.25f, 164, height * 0.35f);

    saveButton.setBounds(x, y, height, height);
    prevButton.setBounds(x + height, y, height, height);
    nextButton.setBounds(x + height * 9.0f, y, height, height);
    presetComboBox.setBounds(x + height * 2.0f, bounds.getY(), height * 7.0f, height);
}

void PresetInterface::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &presetComboBox)
        presetManager.loadPreset(presetComboBox.getItemText(presetComboBox.getSelectedItemIndex()));
}

void PresetInterface::buttonClicked(juce::Button* buttonClicked)
{
    if (buttonClicked == &saveButton){
        fileChooser = std::make_unique<juce::FileChooser>(
            "Enter Preset Name",
            presetManager.defaultDirectory,
            "*." + presetManager.extension);
        
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
        {
            const auto resultFile = chooser.getResult();
            presetManager.savePreset(resultFile.getFileNameWithoutExtension());
            loadPresetList();
        });

    } else if (buttonClicked == &nextButton){
        presetManager.loadNextPreset();
        loadPresetList();
        
    } else if (buttonClicked == &prevButton){
        presetManager.loadPreviousPreset();
        loadPresetList();
    }
}

void PresetInterface::loadPresetList()
{
    presetComboBox.clear(juce::dontSendNotification);
    const auto allPresets = presetManager.getAllPreset();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetComboBox.addItemList(allPresets, 1);
    presetComboBox.setTitle(currentPreset);
    presetComboBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
}
