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


class AlgorithmHelper
{
public:
    std::array<float, 4> toBinary4(int input)
   {
       std::array<float, 4> bits;
       for (int i = 0; i < 4; ++i)
           bits[i] = (input >> i) & 1;
       return bits;
   }
    
    int fromBinary4(const std::array<float, 4>& bits)
    {
        int result = 0;
        for (int i = 0; i < 4; ++i)
        {
            if (bits[i] >= 0.5f)
           //     result |= (1 << (3 - i));
            result |= (1 << i);

        }
        return result;
    }
};

class PatchCable : public juce::Component
{
public:
    PatchCable()
    {
        isInUse = false;
        isConnected = false;
    }
    
    void paint(juce::Graphics& g) override
    {
        juce::Path cablePath, cableEndPath;
        g.setColour(Colors::cableColor);

        if (isInUse){
            // draw output arc and path start
            cableEndPath.addCentredArc(outputPoint.x, outputPoint.y, 2, 2, 0.0f, 0.0f, 6.28f, true);
            g.fillPath(cableEndPath);
            cablePath.startNewSubPath(outputPoint);
            
            float cableWidth = isSelected || isHoveredOn ? 2.0f : 1.0f;
            if (isConnected)
            {
                float dx = inputPoint.x - outputPoint.x;
                float dy = inputPoint.y - outputPoint.y;
                float slack = std::abs(dy) * 0.5f + 20.0f;

                
                juce::Point<float> bezierCoords1(outputPoint.x + dx * 0.15f, outputPoint.y + slack);
                juce::Point<float> bezierCoords2(inputPoint.x - dx * 0.15f, inputPoint.y + 20.0f);

                // draw to input point and its arc
                cablePath.cubicTo(bezierCoords1, bezierCoords2, inputPoint);
                g.strokePath(cablePath, juce::PathStrokeType(cableWidth));
                cableEndPath.addCentredArc(inputPoint.x, inputPoint.y, 2, 2, 0.0f, 0.0f, 6.28f, true);
                g.fillPath(cableEndPath);
                
            } else {
                float dx = mousePoint.x - outputPoint.x;
                float dy = mousePoint.y - outputPoint.y;
                float slack = std::abs(dy) * 0.5f + 40.0f;
                juce::Point<float> bezierCoords1(outputPoint.x + dx * 0.15f, outputPoint.y + slack);
                juce::Point<float> bezierCoords2(mousePoint.x - dx * 0.15f, mousePoint.y + 40.0f);

                // draw to mouse
                cablePath.cubicTo(bezierCoords1, bezierCoords2, mousePoint);
                g.strokePath(cablePath, juce::PathStrokeType(cableWidth));
                cableEndPath.addCentredArc(mousePoint.x, mousePoint.y, 2, 2, 0.0f, 0.0f, 6.28f, true);
                g.fillPath(cableEndPath);
            }
            cableSelectPath = cablePath;
            g.strokePath(cablePath, juce::PathStrokeType(cableWidth));
        }
    }
    
    void resized() override {}
    
    void setOutputPoint(juce::Point<float> point)
    {
        outputPoint = point;
        repaint();
    }
    
    void setInputPoint(juce::Point<float> point)
    {
        inputPoint = point;
        repaint();
    }
    
    void setMousePoint(juce::Point<float> point)
    {
        mousePoint = point;
        repaint();
    }
    
    void setIsCableSelected(bool isSelected)
    {
        this->isSelected = isSelected;
    }

    bool getIsMouseOver(juce::Point<float> mouse)
    {
        return cableSelectPath.contains(mouse, 3.0f);
    }
    
    bool getIsCableSelected()
    {
        return isSelected;
    }
    
    void setIsCableHoveredOn(bool isHoveredOn)
    {
        this->isHoveredOn = isHoveredOn;
    }
    
    void setIsConnected(bool isConnected)
    {
        this->isConnected = isConnected;
    }
    
    void setIsInUse(bool isInUse)
    {
        this->isInUse = isInUse;
    }
    
    bool getIsInUse()
    {
        return isInUse;
    }
    
    bool getIsConnected()
    {
        return isConnected;
    }
    
    void setCableOutputIndex(int outputIndex)
    {
        this->outputIndex = outputIndex;
    }
    
    int getCableOutputIndex()
    {
        return outputIndex;
    }
    
    void setCableInputIndex(int inputIndex)
    {
        this->inputIndex = inputIndex;
    }
    
    int getCableInputIndex()
    {
        return inputIndex;
    }

    
    
private:
    juce::Path cableSelectPath;
    int outputIndex = -1, inputIndex = -1;
    bool isConnected = false, isInUse = false, isSelected = false, isHoveredOn = false;
    juce::Point<float> outputPoint, inputPoint, mousePoint;
    
};



class OperatorBlock : public juce::Component, AlgorithmHelper
{

public:
    void setIsOutput(bool isOutput)
    {
        this->isOutput = isOutput;
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        bounds.reduce(5, 5);
        
        blockSize = bounds.getWidth() * 0.165f;
        blockRectangle = { blockCenterCoords.x - blockSize/2,
            blockCenterCoords.y - blockSize/2,
            blockSize, blockSize };
        juce::Path frontPath, leftSidePath, rightSidePath, botSidePath, topSidePath;
        
        calculatePerspective();
        
        if (!isOutput){
            drawBlockBackground(g);
            drawBlockPoint(g, blockRectangle.getCentreX(), blockRectangle.getY() - 8.0f);
            drawBlockPoint(g, blockRectangle.getCentreX(), blockRectangle.getY() + blockRectangle.getHeight() + 8.0f);
            drawBlockForeground(g);
            g.setColour(juce::Colour(120, 120, 120));
            g.drawText(juce::String(operatorIndex + 1), blockRectangle, juce::Justification::centred);

        } else {
            drawBlockPoint(g, blockRectangle.getCentreX(), blockRectangle.getY() - 8.0f);
            g.setColour(juce::Colour(120, 120, 120));
            g.drawText("Output", blockRectangle.getX(), blockRectangle.getY() - 16.0f, blockRectangle.getWidth(), blockRectangle.getHeight(), juce::Justification::centred);
        }
        
    }
    
    juce::Point<float> interpolateToVanishing(juce::Point<float> origin)
    {
        
        juce::Point<float> interpolatedPoint {
            origin.x + (vanPoint.x - origin.x) * perspective,
            origin.y + (vanPoint.y - origin.y) * perspective };
        
        return interpolatedPoint;
    }
    
    void calculatePerspective()
    {
        perspectiveTopLeft = interpolateToVanishing(blockRectangle.getTopLeft());
        perspectiveTopRight = interpolateToVanishing(blockRectangle.getTopRight());
        perspectiveBotLeft = interpolateToVanishing(blockRectangle.getBottomLeft());
        perspectiveBotRight = interpolateToVanishing(blockRectangle.getBottomRight());
    }

    
    juce::Path createSidePath(juce::Point<float> point1, juce::Point<float> point2, juce::Point<float> point3, juce::Point<float> point4)
    {
        juce::Path path;
        path.startNewSubPath(point1);
        path.lineTo(point2);
        path.lineTo(point3);
        path.lineTo(point4);
        path.closeSubPath();
        return path;
    }
    
    void drawBlockPoint(juce::Graphics &g, float x, float y)
    {
        juce::Path pointPath, outlinePath;
        
        pointPath.addCentredArc(x, y, 2.5f, 2.5f, 0.0f, 0.0f, 6.28f, true);
        g.setColour(Colors::mainColors[4]);
        g.fillPath(pointPath);
        
        if (pointInFocus)
        {
            outlinePath.addCentredArc(x, y, 8.0f, 8.0f, 0.0f, 0.0f, 6.28f, true);
            g.setColour(Colors::mainColors[4]);
            g.strokePath(outlinePath, juce::PathStrokeType(1.0f));
        }
        
    }
    
    void drawBlockBackground(juce::Graphics &g)
    {
        juce::Path leftSidePath, rightSidePath, botSidePath, topSidePath;
        
        leftSidePath = createSidePath(blockRectangle.getTopLeft(), perspectiveTopLeft, perspectiveBotLeft, blockRectangle.getBottomLeft());
        leftSidePath = leftSidePath.createPathWithRoundedCorners(1.0f);
        
        rightSidePath = createSidePath(blockRectangle.getTopRight(), perspectiveTopRight, perspectiveBotRight, blockRectangle.getBottomRight());
        rightSidePath = rightSidePath.createPathWithRoundedCorners(1.0f);

        botSidePath = createSidePath(blockRectangle.getBottomLeft(), blockRectangle.getBottomRight(), perspectiveBotRight, perspectiveBotLeft);
        botSidePath = botSidePath.createPathWithRoundedCorners(1.0f);

        topSidePath = createSidePath(blockRectangle.getTopLeft(), blockRectangle.getTopRight(), perspectiveTopRight, perspectiveTopLeft);
        topSidePath = topSidePath.createPathWithRoundedCorners(1.0f);

        g.setColour(Colors::mainColors[operatorIndex]);
        g.strokePath(leftSidePath, juce::PathStrokeType(1.0f));
        g.strokePath(rightSidePath, juce::PathStrokeType(1.0f));
        g.strokePath(topSidePath, juce::PathStrokeType(1.0f));
        g.strokePath(botSidePath, juce::PathStrokeType(1.0f));

        if (!blockInFocus)
        {
            g.fillPath(leftSidePath);
            g.fillPath(rightSidePath);
            g.fillPath(topSidePath);
            g.fillPath(botSidePath);
            
        }
    }
    
    void drawBlockForeground(juce::Graphics &g)
    {
        // foreground block
        juce::Path graphicPath;
        graphicPath.addRectangle(blockRectangle);
        graphicPath = graphicPath.createPathWithRoundedCorners(1.0f);
        
        g.setColour(Colors::mainColors[operatorIndex]);
        g.strokePath(graphicPath, juce::PathStrokeType(1.0f));
        
        if (!blockInFocus)
        {
            g.setColour(juce::Colour(40, 42, 41));
            g.fillPath(graphicPath);
        }
        
        /*
        g.setFont(juce::FontOptions(30.0f, juce::Font::plain));
        g.drawText(juce::String(operatorIndex + 1),
                   blockRectangle,
                   juce::Justification::centred);
         */
    }
    
    
    void setVanishingPoint(juce::Point<float> vanPoint, float perspective)
    {
        this->perspective = perspective;
        this->vanPoint = vanPoint;
        repaint();
    }
    
    void drawPatchPoint(juce::Graphics& g, float x, float y)
    {
        juce::Path graphicPath;
        graphicPath.addCentredArc(x, y, 4.0f, 4.0f, 0.0f, 0.0f, 6.28f, true);
        g.setColour(Colors::mainColors[4]);
        g.strokePath(graphicPath, juce::PathStrokeType(1.0f));
    }
    
    //******* Sets interaction with mouse
    
    void setBlockCenter(float x, float y)
    {
        blockCenterCoords.x = x;
        blockCenterCoords.y = y;
        
        // refresh source block
        blockRectangle = { blockCenterCoords.x - blockSize/2,
            blockCenterCoords.y - blockSize/2,
            blockSize, blockSize };
        
        repaint();
    }
    
    juce::Point<float> getBlockCenter()
    {
        return blockCenterCoords;
    }

    bool isOverBlock(juce::Point<float> mouse)
    {
        // probably should encapsulate this
        juce::Rectangle blockRectangle = { blockCenterCoords.x - blockSize/2,
            blockCenterCoords.y - blockSize/2,
            blockSize, blockSize };
        
        if (!isOutput){ return blockRectangle.contains(mouse); }
        else { return false; }
    }
    
    bool isOverOutputPoint(juce::Point<float> mouse)
    {
        float pointArea = 10.0f;
                
        juce::Rectangle outputPoint(blockRectangle.getCentreX() - pointArea/2, blockRectangle.getY() + blockRectangle.getHeight() - pointArea/2 + 8.0f, pointArea, pointArea);
        
        return outputPoint.contains(mouse);
    }
    
    bool isOverInputPoint(juce::Point<float> mouse)
    {
        float pointArea = 10.0f;
        juce::Rectangle inputPoint(blockRectangle.getCentreX() - pointArea/2, blockRectangle.getY() - pointArea/2 - 8.0f, pointArea, pointArea);
        
        return inputPoint.contains(mouse);
    }

    juce::Point<float> getInputPoint()
    {
        juce::Point<float> point(blockRectangle.getCentreX(), blockRectangle.getY() - 8.0f);
        return point;
    }
    
    juce::Point<float> getOutputPoint()
    {
        juce::Point<float> point(blockRectangle.getCentreX(), blockRectangle.getY() + blockRectangle.getHeight() + 8.0f);
        return point;
    }

    
    void setBlockInFocus(bool focus)
    {
        blockInFocus = focus;
    }
    
    void setPointInFocus(bool focus)
    {
        pointInFocus = focus;
    }

    void resized() override {}
    
    
    
    std::array<float, 4>& getInputIndex()
    {
        return inputIndex;
    }
    
    void setInput(int index, float value) // potentially with bool
    {
        inputIndex[index] = value;
    //    DBG(inputIndex[0] << inputIndex[1] << inputIndex[2] << inputIndex[3]);
    }
    
    void setInput(std::array<float, 4> inputIndex)
    {
        this->inputIndex = inputIndex;
     //   DBG(inputIndex[0] << inputIndex[1] << inputIndex[2] << inputIndex[3]);
    }
    
    void setInput(int integerIndex)
    {
        inputIndex = toBinary4(integerIndex);
    //    DBG("operator " << juce::String(operatorIndex) << ": " << inputIndex[0] << inputIndex[1] << inputIndex[2] << inputIndex[3]);
    }



    int getOperatorIndex()
    {
        return operatorIndex;
    }

    void setOperatorIndex(int operatorIndex)
    {
        this->operatorIndex = operatorIndex;
    }
    
    void setNumCableAvailable(int amount)
    {
        numCableAvailable += amount;
        
        if (numCableAvailable >= 4) {
            numCableAvailable = 4;
        } else if (numCableAvailable <= 0) {
            numCableAvailable = 0;
        }
    }
    
    int getNumCableAvailable()
    {
        return numCableAvailable;
    }
    
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

class AlgorithmGraphics : public juce::Component, juce::Timer, juce::Button::Listener, AlgorithmHelper
{
public:
    
    AlgorithmGraphics(FledgeAudioProcessor& p) : audioProcessor(p)
    {
        addAndMakeVisible(op[4]);
        op[4].setOperatorIndex(4);
        op[4].setInterceptsMouseClicks(false, false);
        op[4].setIsOutput(true);

        for (int i = 0; i < 4; i++)
        {
            addAndMakeVisible(op[i]);
            op[i].setInterceptsMouseClicks(false, false);
            op[i].setOperatorIndex(i);
            op[i].setIsOutput(false);
            
            for (int j = 0; j < 4; j++){
                addAndMakeVisible(cable[i][j]);
                cable[i][j].setInterceptsMouseClicks(false, false);
                cable[i][j].setAlwaysOnTop(true);
            }
        }
        
        for (int i = 0; i < 16; i++)
        {
            int j = i % 4;
            int k = i / 4;
            addAndMakeVisible(cable[k][j]);
            cable[k][j].setInterceptsMouseClicks(false, false);
        }
        
        setGUIFromParameter();
        addAndMakeVisible(clearCablesButton);
        clearCablesButton.addListener(this);
        
        setWantsKeyboardFocus(true);
        startTimerHz(60);
    }
    
    ~AlgorithmGraphics()
    {
        clearCablesButton.removeListener(this);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        calculateCoordinates(bounds);
        
        op[0].setVanishingPoint(vp, 0.1f);
        op[1].setVanishingPoint(vp, 0.1f);
        op[2].setVanishingPoint(vp, 0.1f);
        op[3].setVanishingPoint(vp, 0.1f);
        
        drawGridBox(g);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        juce::Point<float> vp = bounds.getCentre().toFloat();
        calculateCoordinates(bounds.toFloat());
        
        auto localBounds = getLocalBounds().toFloat();
        clearCablesButton.setBounds(localBounds.getX() + localBounds.getWidth() * 0.75f,
                                    localBounds.getY(),
                                    localBounds.getWidth() * 0.15f,
                                    localBounds.getHeight() * 0.05f);
        
        op[0].setBlockCenter(x + blockIncr * 3, y + blockIncr * 3);
        op[1].setBlockCenter(x + blockIncr * 2, y);
        op[2].setBlockCenter(x + blockIncr, y + blockIncr);
        op[3].setBlockCenter(x + blockIncr * 2, y + blockIncr * 2);

        op[4].setBlockCenter(x + widthMargin + width/2, y + height * 1.075f); // output
        averageVanishingPoint();
        
        for (int i = 0; i <= 4; i++){
            op[i].setBounds(bounds);
            op[i].setVanishingPoint(vp, 0.1f);
            
            for (int j = 0; j < 4; j++)
            {
                cable[i][j].setBounds(bounds);
            }
        }
    }
    
    void drawGridBox(juce::Graphics& g)
    {
        juce::Rectangle<float> frontRectangle = getLocalBounds().toFloat();
        frontRectangle.reduce(5.0f, 5.0f);
        
        juce::Path backPath, frontPath, sidePath;
        float backBoxWidth = width * 0.85f;
        float backBoxHeight = height * 0.85f;
        g.setColour(juce::Colour(80, 80, 80));

        float maxY = frontRectangle.getBottom() - backBoxHeight;
        float constrainedY = juce::jlimit(frontRectangle.getY(), maxY, vp.y - backBoxHeight / 2);
        float constrainedX = juce::jlimit(frontRectangle.getX(),
                                           frontRectangle.getRight() - backBoxWidth,
                                           vp.x - backBoxWidth / 2);

        juce::Rectangle<float> backRectangle = { constrainedX, constrainedY, backBoxWidth, backBoxHeight };

        backPath.addRoundedRectangle(backRectangle, 1.0f);
        g.strokePath(backPath, juce::PathStrokeType(1.0f));
        
        frontPath.addRoundedRectangle(frontRectangle, 1.0f);
        g.strokePath(frontPath, juce::PathStrokeType(1.0f));
        
        sidePath.startNewSubPath(backRectangle.getTopLeft());
        sidePath.lineTo(frontRectangle.getTopLeft());
        
        sidePath.startNewSubPath(backRectangle.getTopRight());
        sidePath.lineTo(frontRectangle.getTopRight());

        sidePath.startNewSubPath(backRectangle.getBottomLeft());
        sidePath.lineTo(frontRectangle.getBottomLeft());

        sidePath.startNewSubPath(backRectangle.getBottomRight());
        sidePath.lineTo(frontRectangle.getBottomRight());

        g.strokePath(sidePath, juce::PathStrokeType(1.0f));
    }
    

    void averageVanishingPoint()
    {
        juce::Point<float> point = { 0.0f, 0.0f };
        for (int i = 0; i < 4; i++)
        {
            point.x += op[i].getBlockCenter().x;
            point.y += op[i].getBlockCenter().y;

        }
        point /= 4.0f;
        auto limitBounds = getLocalBounds().toFloat();
        limitBounds.reduce(limitBounds.getWidth() * 0.2f, limitBounds.getHeight() * 0.2f);
        
        if (limitBounds.contains(point)){
            vp = point;
        } else {
            vp = limitBounds.getConstrainedPoint(point);
        }
    }
    
    void setFromAlgorithmSelection(int selectionIndex)
    {
        float widthScale = x + widthMargin;
        float heightScale = y + heightMargin + height;
        
        deleteAllCables();
        switch(selectionIndex)
        {
            case 0:
                moveBlock(3, { widthScale + width * 0.5f, heightScale * 0.125f });
                moveBlock(2, { widthScale + width * 0.5f, heightScale * 0.35f });
                moveBlock(1, { widthScale + width * 0.5f, heightScale * 0.575f });
                moveBlock(0, { widthScale + width * 0.5f, heightScale * 0.8f });
                setCable(3, 0, 2);
                setCable(2, 0, 1);
                setCable(1, 0, 0);
                setCable(0, 0, 4);
                break;

            case 1:
                moveBlock(3, {widthScale + width * 0.775f, heightScale * 0.175f });
                moveBlock(2, {widthScale + width * 0.775f, heightScale * 0.475f });
                moveBlock(1, {widthScale + width * 0.225f, heightScale * 0.475f });
                moveBlock(0, {widthScale + width * 0.5f, heightScale * 0.775f });
                setCable(3, 0, 2);
                setCable(2, 0, 0);
                setCable(1, 0, 0);
                setCable(0, 0, 4);
                break;

            case 2:
                moveBlock(3, {widthScale + width * 0.225f, heightScale * 0.4f });
                moveBlock(2, {widthScale + width * 0.5f, heightScale * 0.4f });
                moveBlock(1, {widthScale + width * 0.775f, heightScale * 0.4f });
                moveBlock(0, {widthScale + width * 0.5f, heightScale * 0.65f });
                setCable(3, 0, 0);
                setCable(2, 0, 0);
                setCable(1, 0, 0);
                setCable(0, 0, 4);
                break;

            case 3:
                moveBlock(3, {widthScale + width * 0.5f, heightScale * 0.15f });
                moveBlock(2, {widthScale + width * 0.225f, heightScale * 0.4f });
                moveBlock(1, {widthScale + width * 0.775f, heightScale * 0.4f });
                moveBlock(0, {widthScale + width * 0.5f, heightScale * 0.65f });
                setCable(3, 0, 2);
                setCable(3, 1, 1);
                setCable(2, 0, 0);
                setCable(1, 0, 0);
                setCable(0, 0, 4);
                break;
                
            case 4:
                moveBlock(3, {widthScale + width * 0.75f, heightScale * 0.3f });
                moveBlock(2, {widthScale + width * 0.75f, heightScale * 0.65f });
                moveBlock(1, {widthScale + width * 0.25f, heightScale * 0.3f });
                moveBlock(0, {widthScale + width * 0.25f, heightScale * 0.65f });
                setCable(3, 0, 2);
                setCable(2, 0, 4);
                setCable(1, 0, 0);
                setCable(0, 0, 4);
                break;

            case 5:
                moveBlock(3, {widthScale + width * 0.5f, heightScale * 0.15f });
                moveBlock(2, {widthScale + width * 0.5f, heightScale * 0.4f });
                moveBlock(1, {widthScale + width * 0.775f, heightScale * 0.65f });
                moveBlock(0, {widthScale + width * 0.225f, heightScale * 0.65f });
                setCable(3, 1, 2);
                setCable(2, 0, 1);
                setCable(1, 0, 4);
                setCable(2, 1, 0);
                setCable(0, 0, 4);
                break;

            case 6:
                moveBlock(3, {widthScale + width * 0.775f, heightScale * 0.4f });
                moveBlock(2, {widthScale + width * 0.775f, heightScale * 0.65f });
                moveBlock(1, {widthScale + width * 0.5f, heightScale * 0.65f });
                moveBlock(0, {widthScale + width * 0.225f, heightScale * 0.65f });
                setCable(3, 1, 2);
                setCable(2, 0, 4);
                setCable(1, 0, 4);
                setCable(0, 0, 4);
                break;
            case 7:
                moveBlock(3, {widthScale + width * 0.9f, heightScale * 0.6f });
                moveBlock(2, {widthScale + width * 0.625f, heightScale * 0.6f });
                moveBlock(1, {widthScale + width * 0.375f, heightScale * 0.6f });
                moveBlock(0, {widthScale + width * 0.1f, heightScale * 0.6f });
                setCable(3, 1, 4);
                setCable(2, 0, 4);
                setCable(1, 0, 4);
                setCable(0, 0, 4);
                break;
        };
        
        averageVanishingPoint();
    }
    
    void buttonClicked(juce::Button* button) override
    {
        if (button == &clearCablesButton)
        {
            deleteAllCables();
            clearAllInputs();
        }
    }


    void mouseDown(const juce::MouseEvent& m) override
    {
        grabKeyboardFocus();
        
        currentCableIndex.reset();
        currentOutputBlockIndex.reset();
        dragState.reset();
        
        for (int i = 0; i < 4; i++)
        {
            auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
            selectCable(mouse); // organize this later
            
            for (int j = 0; j < 4; j++)
            {
                 cable[i][j].setIsCableSelected(cable[i][j].getIsMouseOver(mouse));
                
                if (op[i].isOverOutputPoint(mouse) && !cable[i][j].getIsInUse())
                {
                    currentOutputBlockIndex = i;
                    currentCableIndex = j;

                    startNewCableOnMouseDown(i, j, mouse);
                    dragState = 2; // dragging cable
                    return;
                }
            }
            
            if (op[i].isOverBlock(mouse)) {
                moveBlock(i, mouse);
                followCable(i);

                op[i].setBlockInFocus(true);
                currentOutputBlockIndex = i;
                dragState = 1; // dragging block
                return;
            }
            
            if (op[i].isOverOutputPoint(mouse) || op[i].isOverInputPoint(mouse) || op[i].isOverBlock(mouse)){
                dragState = 0; // not over anything
                return;
            }
        }
    }
    
    void mouseDrag(const juce::MouseEvent& m) override
    {
        int blk = *currentOutputBlockIndex;
        int cbl = *currentCableIndex;

        for (int i = 0; i <= 4; i++)
        {
            auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
            
            if (currentOutputBlockIndex.has_value() && *dragState == 1) // dragging block
            {
                mouse = limitBlockDrag(mouse);
                moveBlock(blk, mouse);
                followCable(blk);
                op[blk].setBlockInFocus(true);
                averageVanishingPoint();
            }
            
            if (currentCableIndex.has_value() && *dragState == 2) // dragging cable end
            {
                auto outputPoint = op[blk].getOutputPoint();
                cable[blk][cbl].setIsInUse(true);
                cable[blk][cbl].setOutputPoint(outputPoint);
                cable[blk][cbl].setMousePoint(mouse);
            }
            
            if (op[i].isOverOutputPoint(mouse) && *dragState == 2)
            {
                op[i].setPointInFocus(true);
            }
            
            if (op[i].isOverInputPoint(mouse) && *dragState == 2)
            {
                op[i].setPointInFocus(true);
            }
        }
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        int blk = *currentOutputBlockIndex;
        int cbl = *currentCableIndex;

        for (int i = 0; i <= 4; i++)
        {
            auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
            if (currentCableIndex.has_value() && op[i].isOverInputPoint(mouse) && *dragState == 2)
            {
                auto inputPoint = op[i].getInputPoint();
                cable[blk][cbl].setInputPoint(inputPoint);
                cable[blk][cbl].setCableInputIndex(i);
                cable[blk][cbl].setCableOutputIndex(blk);
                cable[blk][cbl].setIsInUse(true);
                cable[blk][cbl].setIsConnected(true);
                
                
                op[i].setInput(blk, 1.0f);
                setParameterFromGUI();
                break;
            }
            
            if (currentCableIndex.has_value() && !op[i].isOverInputPoint(mouse) && *dragState == 2)
            {
                cable[blk][cbl].setIsInUse(false);
                cable[blk][cbl].setIsConnected(false);
            }
        }

        
        for (int i = 0; i <= 4; i++)
        {
            op[i].setBlockInFocus(false);
            op[i].setPointInFocus(false);
        }
        
        currentCableIndex.reset();
        currentOutputBlockIndex.reset();
        dragState.reset();
    }

    void moveBlock(int blockToMove, juce::Point<float> newPoint)
    {
        // drag block
        op[blockToMove].setBlockCenter(newPoint.x, newPoint.y);
    }
    
    void followCable(int blockToMove)
    {
        auto outputPoint = op[blockToMove].getOutputPoint();
        setCableOutputCoords(blockToMove, outputPoint);
        
        auto inputPoint = op[blockToMove].getInputPoint();
        setCableInputCoords(blockToMove, inputPoint);
    }
    
    void startNewCableOnMouseDown(int originBlock, int cableIndex, juce::Point<float> mouse)
    {
        if (op[originBlock].getNumCableAvailable() != 0)
        {
            auto outputPoint = op[originBlock].getOutputPoint();
            cable[originBlock][cableIndex].setOutputPoint(outputPoint);
            cable[originBlock][cableIndex].setMousePoint(mouse);
            cable[originBlock][cableIndex].setIsInUse(true);
            cable[originBlock][cableIndex].setIsConnected(false);
        }
    }
    
    void setCable(int originBlock, int cableIndex, int destBlock)
    {
        auto outputPoint = op[originBlock].getOutputPoint();
        cable[originBlock][cableIndex].setOutputPoint(outputPoint);

        auto inputPoint = op[destBlock].getInputPoint();
        cable[originBlock][cableIndex].setInputPoint(inputPoint);
        
        cable[originBlock][cableIndex].setIsInUse(true);
        cable[originBlock][cableIndex].setIsConnected(true);
        
        cable[originBlock][cableIndex].setCableInputIndex(destBlock);
        cable[originBlock][cableIndex].setCableOutputIndex(originBlock);
    }
    
    void setCableOutputCoords(int originBlock, juce::Point<float> newOutputPoint)
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (originBlock == cable[i][j].getCableOutputIndex()) // origin
                {
                    cable[i][j].setOutputPoint(newOutputPoint);
                }
            }
        }
    }
    
    void setCableInputCoords(int destinationBlock, juce::Point<float> newInputPoint)
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (destinationBlock == cable[i][j].getCableInputIndex())
                {
                    cable[i][j].setInputPoint(newInputPoint);
                }
            }
        }
    }

    juce::Point<float> limitBlockDrag(juce::Point<float> mousePoint)
    {
        auto bounds = getLocalBounds().toFloat();
        float xOffset = bounds.getX() + bounds.getWidth() * 0.05f;
        float yOffset = bounds.getY() + bounds.getHeight() * 0.15f;
        float widthScaled = bounds.getWidth() * 0.9f;
        float heightOffset = bounds.getWidth() * 0.85f;

        bounds.setX(xOffset);
        bounds.setY(yOffset);
        bounds.setWidth(widthScaled);
        bounds.setHeight(heightOffset);

        
        if (bounds.contains(mousePoint)) { return mousePoint; }
        else { return bounds.getConstrainedPoint(mousePoint); }
    }
    
    void connectCableToBlock(int originBlock, int cableToConnect, int destinationBlock)
    {
        
    }
    
    void disconnectCableFromBlock(int cableToDisconnect, int disconnectedBlock)
    {
        
    }
    
    void setGUIFromParameter()
    {
        /*
        // do this later
    //    auto op0Routing = audioProcessor.apvts.getRawParameterValue("operator0Routing")->load();
        auto output = toBinary4(1);
        for (int i = 0; i < 4; i++)
        {
            if (output[i] > 0){
                setCable(0, i, 2);
            }
        }
         */
    }
    
    void setParameterFromGUI()
    {
        // set output
        int outputGainInt = fromBinary4(op[4].getInputIndex());
        auto outputParam = audioProcessor.params->apvts.getParameter("outputRouting");
        
        if (outputParam != nullptr){
            float outputGainFloat = outputParam->convertTo0to1(outputGainInt);
            outputParam->setValueNotifyingHost(outputGainFloat);
        } else {
            DBG("Parameter not found for output routing");
        }
        
        // set operators
        for (int i = 0; i < 4; i++) {
            int operatorGainInt = fromBinary4(op[i].getInputIndex());
            auto operatorParam = audioProcessor.params->apvts.getParameter("operator" + juce::String(i) + "Routing");
            
            if (operatorParam != nullptr){
                float operatorGainFloat = operatorParam->convertTo0to1(operatorGainInt);
                operatorParam->setValueNotifyingHost(operatorGainFloat);
            } else {
                DBG("Parameter not found for index: " << i);
            }
        }
    }
    
    void selectCable(juce::Point<float> mouse)
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                bool cableSelected = cable[i][j].getIsMouseOver(mouse);
                cable[i][j].setIsCableSelected(cableSelected);
            }
        }
    }
    
    void deleteAllCables()
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cable[i][j].setIsInUse(false);
                cable[i][j].setIsConnected(false);
                cable[i][j].setCableInputIndex(-1);
                cable[i][j].setCableOutputIndex(-1);
            }
        }
    }
    
    void clearAllInputs()
    {
        for (int i = 0; i <= 4; i++) {
                op[i].setInput(0);
        }
    }

    void deleteSelectedCable()
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if(cable[i][j].getIsCableSelected())
                {
                    cable[i][j].setIsInUse(false);
                    cable[i][j].setIsConnected(false);
                    
                    
                    // when cable is detached, it modifies the destination
                    int outputIndex = cable[i][j].getCableOutputIndex(); // cable origin
                    int inputIndex = cable[i][j].getCableInputIndex(); // cable destination

                    auto blockInputIndex = op[inputIndex].getInputIndex(); // array of
                    DBG("recieved input block to modify: " << inputIndex);
                    DBG("before: " << blockInputIndex[0] << blockInputIndex[1] << blockInputIndex[2] << blockInputIndex[3]);

                    blockInputIndex[outputIndex] = 0.0f;
                    DBG("modified output index: " << outputIndex);
                    DBG("after: " << blockInputIndex[0] << blockInputIndex[1] << blockInputIndex[2] << blockInputIndex[3]);

                    int modifiedInputIndex = fromBinary4(blockInputIndex);
                    op[inputIndex].setInput(modifiedInputIndex);
                    cable[i][j].setCableOutputIndex(-1);

                }
            }
        }
    }

    void calculateCoordinates(juce::Rectangle<float> bounds)
    {
        x = bounds.getX();
        y = bounds.getY();
        widthMargin = bounds.getWidth() * 0.05f;
        heightMargin = bounds.getHeight() * 0.05f;
        height = bounds.getHeight() * 0.9f;
        width = bounds.getWidth() * 0.9f;
        blockIncr = width * 0.15f;
    }
        
    void timerCallback() override
    {
        auto mouse = getMouseXYRelative().toFloat();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                
                // hovering on cable
                cable[i][j].setIsCableHoveredOn(cable[i][j].getIsMouseOver(mouse));
                
            }
        }
    }
    
    bool keyPressed(const juce::KeyPress& key) override
    {
        if (key.getKeyCode() == juce::KeyPress::backspaceKey)
        {
            deleteSelectedCable();
        //    clearAllInputs();
            setParameterFromGUI();
        }
        return true;
    }

    
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


class BlockDiagrams : public juce::LookAndFeel_V4
{
public:
    
    void setIndex(int graphicIndex)
    {
        this->graphicIndex = graphicIndex;
        selectAlgorithm();
    }
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        drawAlgorithm(g, bounds.getX(), bounds.getY(), bounds.getWidth(), shouldDrawButtonAsDown);
    }
    
    void selectAlgorithm()
    {
        switch(graphicIndex){
            case 0:
                block.blockToUse = { 1, 5, 9, 13 };
                block.connectValue = { DOWN, DOWN, DOWN, DOWN };
                block.label = { "4", "3", "2", "1" };
                break;
                
            case 1:
                block.blockToUse = { 7, 9, 11, 14 };
                block.connectValue = { DOWN, DOWNRIGHT, DOWNLEFT, DOWN };
                block.label = { "4", "3", "2", "1" };
                break;
                
            case 2:
                block.blockToUse = { 9, 10, 11, 14 };
                block.connectValue = { DOWNRIGHT, DOWN, DOWNLEFT, DOWN };
                block.label = { "4", "3", "2", "1" };
                break;
                
            case 3:
                block.blockToUse = { 6, 9, 11, 14 };
                block.connectValue = { DOWNLEFTRIGHT, DOWNRIGHT, DOWNLEFT, DOWN };
                block.label = { "4", "3", "2", "1" };
                break;

            case 4:
                block.blockToUse = { 9, 10, 13, 14 };
                block.connectValue = { DOWN, DOWN, DOWN, DOWN };
                block.label = { "2", "4", "1", "3" };
                break;
                
            case 5:
                block.blockToUse = { 5, 9, 12, 14 };
                block.connectValue = { DOWN, DOWNLEFTRIGHT, DOWN, DOWN };
                block.label = { "4", "3", "1", "2" };
                break;

            case 6:
                block.blockToUse = { 10, 12, 13, 14 };
                block.connectValue = { DOWN, DOWNLEFT, DOWN, DOWN };
                block.label = { "4", "1", "2", "3" };
                break;

            case 7:
                block.blockToUse = { 12, 13, 14, 15 };
                block.connectValue = { DOWN, DOWN, DOWN, DOWN };
                block.label = { "1", "2", "3", "4" };
                break;
        }
    }
    
    
    void drawAlgorithm(juce::Graphics& g, float x, float y, float size, bool mouseDown)
    {
        float graphicSize = size * 0.8f;
        float margin = size * 0.2f;
        float blockSize = (graphicSize/4) * 0.7f;
        float blockMargin = (graphicSize/4) * 0.15f;

        //==============================================================================

        for (int i = 0; i < 16; i++){ // column
            float xIncr = x + margin + (graphicSize/4) * (i % 4);
            float yIncr = y + margin + (graphicSize/4) * (i / 4);
            
            juce::Path blockPath;
            blockPath.addRoundedRectangle(xIncr, yIncr, blockSize, blockSize, 2);
            
            for (int j = 0; j < 4; j++) {
                if (i != block.blockToUse[j]) {
                    g.setColour(juce::Colour(50, 50, 50));
                    g.strokePath(blockPath, juce::PathStrokeType(1.0f));

                } else {
                    auto fillColor = mouseDown ? Colors::mainHoverColors[j] : Colors::mainColors[j];
                    g.setColour(fillColor);
                    g.fillPath(blockPath);
                    g.strokePath(blockPath, juce::PathStrokeType(1.0f));
                    break;
                }

            }
        }
        
        for (int i = 0; i < 16; i++){ // column
            float xIncr = x + margin + (graphicSize/4) * (i % 4);
            float yIncr = y + margin + (graphicSize/4) * (i / 4);
            
            juce::Path linePath;
            for (int j = 0; j < 4; j++) {
                if (i == block.blockToUse[j]){
                    if (block.connectValue[j] == DOWNLEFT){
                        linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                        linePath.lineTo(xIncr + blockSize/2, yIncr + (blockSize + blockMargin) * 1.5f);
                        linePath.lineTo(xIncr - (blockSize + blockMargin), yIncr + (blockSize + blockMargin) * 1.5f);
                        
                    } else if (block.connectValue[j] == DOWNRIGHT){
                        linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                        linePath.lineTo(xIncr + blockSize/2, yIncr + (blockSize + blockMargin) * 1.5f);
                        linePath.lineTo(xIncr + (blockSize + blockMargin) * 1.5f, yIncr + (blockSize + blockMargin) * 1.5f);
                        
                    } else if (block.connectValue[j] == DOWN) {
                        linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                        linePath.lineTo(xIncr + blockSize/2, yIncr + (blockSize + blockMargin) * 1.25f);
                        
                    } else if (block.connectValue[j] == LEFTDOWN) {
                        linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                        linePath.lineTo(xIncr - (blockSize + blockMargin) * 1.5f, yIncr + blockSize/2);
                        linePath.lineTo(xIncr - (blockSize + blockMargin) * 1.5f, yIncr + (blockSize + blockMargin) * 1.5f);
                        
                    } else if (block.connectValue[j] == RIGHTDOWN) {
                        linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                        linePath.lineTo(xIncr + (blockSize + blockMargin) * 1.5f, yIncr + blockSize/2);
                        linePath.lineTo(xIncr + (blockSize + blockMargin) * 1.5f, yIncr + (blockSize + blockMargin) * 1.5f);
                        
                    } else if (block.connectValue[j] == DOWNLEFTRIGHT) {
                        linePath.startNewSubPath(xIncr + blockSize * 0.425f, yIncr + blockSize/2);
                        linePath.lineTo(xIncr + blockSize * 0.425f, yIncr + (blockSize + blockMargin) * 1.5f);
                        linePath.lineTo(xIncr - (blockSize + blockMargin), yIncr + (blockSize + blockMargin) * 1.5f);

                        linePath.startNewSubPath(xIncr + blockSize * 0.575f, yIncr + blockSize/2);
                        linePath.lineTo(xIncr + blockSize * 0.575f, yIncr + (blockSize + blockMargin) * 1.5f);
                        linePath.lineTo(xIncr + (blockSize + blockMargin) * 1.5f, yIncr + (blockSize + blockMargin) * 1.5f);

                    } else if (block.connectValue[j] == NONE) {}
                }
            }
            
            linePath = linePath.createPathWithRoundedCorners(3);
            g.setColour(juce::Colour(90, 224, 184));

            juce::PathStrokeType strokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
            g.strokePath(linePath, juce::PathStrokeType(strokeType));
            
        }
        
        for (int i = 0; i < 16; i++){ // column
            float xIncr = x + margin + (graphicSize/4) * (i % 4);
            float yIncr = y + margin + (graphicSize/4) * (i / 4);
            g.setColour(juce::Colour(40, 40, 40));
            g.setFont(9.0f);
            
            for (int j = 0; j < 4; j++) {
                if (i == block.blockToUse[j]) {
                        g.drawText(block.label[j], xIncr + 0.75f, yIncr + 0.5f, blockSize, blockSize,juce::Justification::centred);
                }
            }
        }
    }

    
    
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
