/*
  ==============================================================================

    AlgorithmGraphics.h
    Created: 5 Jul 2025 11:37:40am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "GraphicsUtility.h"
#include "AlgorithmHelper.h"


class PatchCable : public juce::Component
{
public:
    PatchCable();
    void paint(juce::Graphics& g) override;
    void resized() override {}
    //==============================================================================

    void setOutputPoint(juce::Point<float> point);
    void setInputPoint(juce::Point<float> point);
    void setMousePoint(juce::Point<float> point);
    //==============================================================================

    void setIsCableSelected(bool isSelected);
    bool getIsMouseOver(juce::Point<float> mouse);
    bool getIsCableSelected();
    void setIsCableHoveredOn(bool isHoveredOn);
    //==============================================================================

    void setIsConnected(bool isConnected);
    void setIsInUse(bool isInUse);
    bool getIsInUse();
    bool getIsConnected();
    //==============================================================================

    void setCableOutputIndex(int outputIndex);
    int getCableOutputIndex();
    void setCableInputIndex(int inputIndex);
    int getCableInputIndex();

private:
    juce::Path cableSelectPath;
    int outputIndex = -1, inputIndex = -1;
    bool isConnected = false, isInUse = false, isSelected = false, isHoveredOn = false;
    juce::Point<float> outputPoint, inputPoint, mousePoint;
    
};



class OperatorBlock : public juce::Component, AlgorithmHelper
{

public:
    void setIsOutput(bool isOutput);
    void paint(juce::Graphics& g) override;
    void resized() override {}
    //==============================================================================

    juce::Point<float> interpolateToVanishing(juce::Point<float> origin);
    void calculatePerspective();
    //==============================================================================

    juce::Path createSidePath(juce::Point<float> point1, juce::Point<float> point2, juce::Point<float> point3, juce::Point<float> point4);
    void drawBlockPoint(juce::Graphics &g, float x, float y);
    void drawBlockBackground(juce::Graphics &g);
    void drawBlockForeground(juce::Graphics &g);
    void setVanishingPoint(juce::Point<float> vanPoint, float perspective);
    void drawPatchPoint(juce::Graphics& g, float x, float y);
    //==============================================================================

    void setBlockCenter(float x, float y);
    juce::Point<float> getBlockCenter();
    //==============================================================================

    bool isOverBlock(juce::Point<float> mouse);
    bool isOverOutputPoint(juce::Point<float> mouse);
    bool isOverInputPoint(juce::Point<float> mouse);
    juce::Point<float> getInputPoint();
    juce::Point<float> getOutputPoint();
    //==============================================================================

    void setBlockInFocus(bool focus);
    void setPointInFocus(bool focus);
    //==============================================================================

    std::array<float, 4>& getInputIndex();
    void setInput(int index, float value);
    void setInput(std::array<float, 4> inputIndex);
    void setInput(int integerIndex);
    //==============================================================================

    int getOperatorIndex();
    void setOperatorIndex(int operatorIndex);
    //==============================================================================

    void setNumCableAvailable(int amount);
    int getNumCableAvailable();
    
private:
    float width, x, y;
    
    int numCableAvailable = 4;
    int operatorIndex;
    std::array<float, 4> inputIndex;
    
    // drawing state
    bool blockInFocus;
    bool pointInFocus;
    bool isOutput;
    
    float blockSize;
    float perspective = 0.5f;
    
    juce::Rectangle<float> blockRectangle;
    juce::Point<float> vanPoint,
    blockCenterCoords,
    perspectiveTopLeft,
    perspectiveTopRight,
    perspectiveBotLeft,
    perspectiveBotRight;
};

class AlgorithmGraphics : public juce::Component, juce::Timer, juce::Button::Listener, AlgorithmHelper, GraphicsHelper
{
public:
    
    AlgorithmGraphics(FledgeAudioProcessor& p);
    ~AlgorithmGraphics();
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void calculateCoordinates(juce::Rectangle<float> bounds);
    void averageVanishingPoint();
    
    void setFromAlgorithmSelection(int selectionIndex);
    void buttonClicked(juce::Button* button) override;

    void drawGridBox(juce::Graphics& g, juce::Rectangle<float> bounds);
    


    void mouseDown(const juce::MouseEvent& m) override;
    void mouseDrag(const juce::MouseEvent& m) override;
    void mouseUp(const juce::MouseEvent& m) override;
    
    
    void startNewCableOnMouseDown(int originBlock, int cableIndex, juce::Point<float> mouse);
    void setCable(int originBlock, int cableIndex, int destBlock);
    void setCableOutputCoords(int originBlock, juce::Point<float> newOutputPoint);
    void setCableInputCoords(int destinationBlock, juce::Point<float> newInputPoint);
    void connectCableToBlock(int originBlock, int cableToConnect, int destinationBlock);
    void disconnectCableFromBlock(int cableToDisconnect, int disconnectedBlock);

    void moveBlock(int blockToMove, juce::Point<float> newPoint);
    void followCable(int blockToMove);
    juce::Point<float> limitBlockDrag(juce::Point<float> mousePoint);
    
    void setGUIFromParameter();
    void setParameterFromGUI();
    
    void selectCable(juce::Point<float> mouse);
    void deleteAllCables();
    void clearAllInputs();
    void deleteSelectedCable();
    
    void timerCallback() override;
    bool keyPressed(const juce::KeyPress& key) override;
    
    std::array<OperatorBlock, 5> op;
private:
    juce::Rectangle<float> bounds;
    juce::Point<float> vp;
    float x, y, width, widthMargin, height, heightMargin, blockIncr;
    juce::TextButton clearCablesButton;

    
    std::array<std::array<PatchCable, 4>, 5> cable;
    std::optional<int> currentCableIndex, currentOutputBlockIndex;
    std::optional<int> dragState; // 0 = out of bounds, 1 = dragging block, 2 = dragging cable
    
    FledgeAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AlgorithmGraphics)
};


