/*
  ==============================================================================

    PresetLayout.cpp
    Created: 8 Aug 2025 10:20:57pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "PresetLayout.h"

void PresetInterfaceLookAndFeel::drawButtonBackground (juce::Graphics &g, juce::Button &button, const juce::Colour &backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    fillBackgroundPanel(g, bounds);
    
    bounds.reduce(5, 5);
    float x = bounds.getX();
    float y = bounds.getY();
    float size = bounds.getHeight();
    float graphicWidth = bounds.getWidth();
    
    switch(lookAndFeel)
        case PresetInterfaceLAF::Save:
    {
        drawSaveButton(g, x, y, size);
        break;
    case PresetInterfaceLAF::Prev: {
        drawArrowButton(g, x, y, size, false);
        break;
    }
    case PresetInterfaceLAF::Next: {
        drawArrowButton(g, x, y, size, true);
        break;
    } default:
        break;
    }
}

void PresetInterfaceLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox)
{
    auto bounds = juce::Rectangle<int>(width, height).toFloat();
    fillBackgroundPanel(g, bounds);

    bounds.reduce(5, 5);
    juce::Path buttonPath;
    auto buttonBounds = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH);
    buttonPath.addRoundedRectangle(buttonBounds, 3.0f);
    
    g.setFont(12.0f);
    g.setColour(Colors::textColor);
    g.drawText(comboBox.getText(), bounds, juce::Justification::centred, false);
}

void PresetInterfaceLookAndFeel::drawSaveButton(juce::Graphics& g, float x, float y, float size)
{
    float graphicMargin = size * 0.3f;
    float graphicSize = size * 0.4f;
    x = x + graphicMargin;
    y = y + graphicMargin;

    juce::Point<float> topLeft { x, y };
    juce::Point<float> slopeStart { x + graphicSize * 0.75f, y };
    juce::Point<float> slopeEnd { x + graphicSize, y + graphicSize * 0.25f};
    juce::Point<float> botRight { x + graphicSize, y + graphicSize };
    juce::Point<float> botLeft { x, y + graphicSize };

    juce::Path bodyPath;
    bodyPath.startNewSubPath(topLeft);
    bodyPath.lineTo(slopeStart);
    bodyPath.lineTo(slopeEnd);
    bodyPath.lineTo(botRight);
    bodyPath.lineTo(botLeft);
    bodyPath.closeSubPath();
    bodyPath = bodyPath.createPathWithRoundedCorners(1.0f);
    g.setColour(Colors::mainColors[0]);
    g.fillPath(bodyPath);
}


void PresetInterfaceLookAndFeel::drawArrowButton(juce::Graphics& g, float x, float y, float size, bool isLeftArrow)
{
    float graphicMargin = size * 0.4f;
    float graphicSize = size * 0.2f;
    x = x + graphicMargin;
    y = y + graphicMargin;

    // coordinates
    juce::Point<float> topLeft { x, y };
    juce::Point<float> botLeft { x, y + graphicSize };
    juce::Point<float> middleRight { x + graphicSize, y + graphicSize/2 };
    juce::Point<float> topRight { x + graphicSize, y };
    juce::Point<float> middleLeft { x, y + graphicSize/2 };
    juce::Point<float> botRight { x + graphicSize, y + graphicSize };

    // drawing
    juce::Path arrowPath;
    if (isLeftArrow){
        arrowPath.startNewSubPath(topLeft);
        arrowPath.lineTo(botLeft);
        arrowPath.lineTo(middleRight);
        arrowPath.closeSubPath();
    } else {
        arrowPath.startNewSubPath(topRight);
        arrowPath.lineTo(botRight);
        arrowPath.lineTo(middleLeft);
        arrowPath.closeSubPath();
    }

    arrowPath = arrowPath.createPathWithRoundedCorners(1.0f);
    g.setColour(Colors::mainColors[0]);
    g.fillPath(arrowPath);
}


void PresetInterfaceLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour)
{
    juce::Path menuPath;
    menuPath.addRectangle(area);
    
    juce::Colour menuColor;
    menuColor = isHighlighted ? juce::Colour(40, 42, 41) : juce::Colour(40, 42, 41);
    g.setColour(menuColor);
    g.fillPath(menuPath);
    
    g.setColour(Colors::textColor);
    g.setFont(12.0f);
    g.drawText(text, area, juce::Justification::centred, false);
}

void PresetInterfaceLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    juce::Path menuPath;
    menuPath.addRectangle(0, 0, width, height);
    g.setColour(juce::Colour(40, 42, 41));
    g.fillPath(menuPath);
}

PresetInterface::PresetInterface(FledgeAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts) : presetManager(apvts), audioProcessor(p)
{
    juce::FontOptions font { 12.0f, juce::Font::plain };

    addAndMakeVisible(saveButton);
    saveButton.addListener(this);
    saveButton.setLookAndFeel(&saveLAF);

    addAndMakeVisible(nextButton);
    nextButton.addListener(this);
    nextButton.setLookAndFeel(&nextLAF);

    addAndMakeVisible(prevButton);
    prevButton.addListener(this);
    prevButton.setLookAndFeel(&prevLAF);
    
    addAndMakeVisible(presetComboBox);
    presetComboBox.addListener(this);
    presetComboBox.setLookAndFeel(&comboBoxLAF);
    
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
