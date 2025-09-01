
#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "AlgorithmGraphics.h"
#include "PluginProcessor.h"


class BlockDiagrams : public juce::LookAndFeel_V4
{
public:
    
    void setIndex(int graphicIndex);
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void selectAlgorithm();
    void drawAlgorithm(juce::Graphics& g, float x, float y, float size, bool mouseDown);
private:
    int graphicIndex;
    
    struct blockValues
    {
        std::array<int, 4> blockToUse;
        std::array<int, 4> connectValue;
        std::array<juce::String, 4> label = { "3", "2", "1", "4" };
    };
    enum blockConnect { NONE, DOWN, DOWNLEFT, DOWNRIGHT, LEFTDOWN, RIGHTDOWN, DOWNLEFTRIGHT };
    blockValues block;
    
};

class AlgorithmSelectInterface : public juce::Component, juce::Button::Listener
{
public:
    AlgorithmSelectInterface(FledgeAudioProcessor& p, AlgorithmGraphics& a);
    ~AlgorithmSelectInterface();
    void paint(juce::Graphics& g) override {}
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void setOperatorParam(int index, int gainIndex);
    void setOutputParam(int gainIndex);
    void setPresetParam(int index);
    
    std::array<juce::ToggleButton, 8> algorithm;
private:
    std::array<BlockDiagrams, 8> algorithmGraphics;
    
    AlgorithmGraphics& algoGraphics;
    FledgeAudioProcessor& audioProcessor;
};

