/*
  ==============================================================================

    PresetLayout.h
    Created: 8 Aug 2025 10:20:57pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PresetManager.h"
#include "LookAndFeel.h"
#include "GraphicsUtility.h"

enum class PresetInterfaceLAF { Save, Prev, Next, ComboBox };

class PresetInterfaceLookAndFeel : public juce::LookAndFeel_V4, GraphicsHelper
{
public:
    PresetInterfaceLookAndFeel(PresetInterfaceLAF l) : lookAndFeel(l)
    {
        setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    }
    
    void drawButtonBackground (juce::Graphics &g, juce::Button &button, const juce::Colour &backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox) override;

    void drawSaveButton(juce::Graphics& g, float x, float y, float size);
    void drawArrowButton(juce::Graphics& g, float x, float y, float size, bool isLeftArrow);

    
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) override;
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    void drawResizableFrame(juce::Graphics& g, int w, int h, const juce::BorderSize<int>& b) override {}

private:
    PresetInterfaceLAF lookAndFeel;
};


class PresetInterface : public juce::Component, juce::ComboBox::Listener, juce::Button::Listener
{
public:
    PresetInterface(FledgeAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~PresetInterface();
    
    void paint(juce::Graphics& g) override {}
    
    void resized() override;
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* buttonClicked) override;
    void loadPresetList();
    
private:
    
     
    PresetInterfaceLookAndFeel saveLAF { PresetInterfaceLAF::Save },
    prevLAF{ PresetInterfaceLAF::Prev },
    nextLAF { PresetInterfaceLAF::Next },
    comboBoxLAF { PresetInterfaceLAF::ComboBox };
    
    juce::TextButton saveButton, nextButton, prevButton;
    juce::ComboBox presetComboBox, editorSizeComboBox;
    juce::Label rateLabel, rateValueLabel;
    
    std::unique_ptr<juce::FileChooser> fileChooser;
    PresetManager presetManager;
    FledgeAudioProcessor& audioProcessor;
};

