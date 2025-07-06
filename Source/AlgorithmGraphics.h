/*
  ==============================================================================

    AlgorithmGraphics.h
    Created: 5 Jul 2025 11:37:40am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class PatchCable : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        juce::Path cablePath;
        cablePath.startNewSubPath(outputPoint);
        cablePath.lineTo(mousePoint);
        
        g.setColour(juce::Colour(100, 100, 100));
        g.strokePath(cablePath, juce::PathStrokeType(2.0f));
        
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
    
    bool isCableSelected() { return 1; }
    void setIsConnected(bool isConnected)
    {
        this->isConnected = isConnected;
    }
    
    void setIsInUse(bool isInUse)
    {
        this->isInUse = isInUse;
    }
    
private:
    bool isConnected = false, isInUse = false;
    juce::Point<float> outputPoint, inputPoint, mousePoint;
    
};



class OperatorBlock : public juce::Component
{
    /*
    each block should know:
        connection recieved (from which cable it got)
        cables recieved (from which cable it got)

        set blockIndex
     
    */
public:
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        bounds.reduce(5, 5);
        width = bounds.getWidth();
        x = bounds.getX();
        y = bounds.getY();

        float blockSize = width * 0.9f;
        float blockMargin = width * 0.05f;

        juce::Path graphicPath;
        graphicPath.addRoundedRectangle(x + blockMargin, y + blockMargin, blockSize, blockSize, 4.0f);
        g.setColour(juce::Colour(100, 100, 100));
        g.fillPath(graphicPath);
        
        // input
        drawPatchPoint(g, x + blockMargin + blockSize/2, y + blockMargin);
        // output
        drawPatchPoint(g, x + blockMargin + blockSize/2, y + blockMargin + blockSize);
    }
    
    void drawPatchPoint(juce::Graphics& g, float x, float y)
    {
        juce::Path graphicPath;
        graphicPath.addCentredArc(x, y, 4.0f, 4.0f, 0.0f, 0.0f, 6.28f, true);
        g.setColour(juce::Colour(120, 120, 120));
        g.strokePath(graphicPath, juce::PathStrokeType(2.0f));
    }
    
    
    bool isOverPoint(juce::Point<float> mouse)
    {
        float blockSize = width * 0.9f;
        float blockMargin = width * 0.05f;
        float pointArea = 10.0f;
        
        juce::Rectangle inputArea = { (x + blockMargin + blockSize/2) - pointArea/2 ,
            (y + blockMargin) - pointArea/2,
            pointArea, pointArea };
        
        juce::Rectangle outputArea = { (x + blockMargin + blockSize/2) - pointArea/2 ,
            (y + blockMargin + blockSize) - pointArea/2,
            pointArea, pointArea };

        bool inputBool = inputArea.contains(mouse);
        bool outputBool = outputArea.contains(mouse);
        
        return inputBool || outputBool;
    }
    
    void resized() override {}
    
private:
    float width, x, y;
};



class AlgorithmGraphics : public juce::Component
{
public:
    
    AlgorithmGraphics()
    {
        for (int i = 0; i < 4; i++)
        {
            addAndMakeVisible(op[i]);
            op[i].setInterceptsMouseClicks(true, true);
        }
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float blockIncr = bounds.getWidth()/4.0f;

        op[0].setBounds(x + blockIncr, y, blockIncr, blockIncr);
        op[1].setBounds(x, y + blockIncr, blockIncr, blockIncr);
        op[2].setBounds(x, y + blockIncr * 2, blockIncr, blockIncr);
        op[3].setBounds(x + blockIncr * 3, y + blockIncr * 2, blockIncr, blockIncr);
    }
    
    void resized() override
    {
        
        
    }

    void mouseDown(const juce::MouseEvent& m) override
    {
        bool modifier = m.mods.isCommandDown();
        for (int point = 0; point < 4; point++)
        {
            auto mouse = m.getEventRelativeTo(&op[point]).getPosition().toFloat();
            if (op[point].isOverPoint(mouse))
            {
                DBG("mousedown yes");

                cable[0].setOutputPoint(mouse);
                cable[0].setIsInUse(true);
                currentCableIndex = 0;
                
            }
            /*
            if (op[point].isOverPoint(mouse) && modifier){
                
                DBG("mousedown with modifier");
            }
             */
        }
    }
    
    void mouseDrag(const juce::MouseEvent& m) override
    {
        auto mouse = m.getPosition().toFloat();
        cable[0].setMousePoint(mouse);
        cable[0].setIsInUse(true);
        DBG("drag");
            
            
            
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        currentCableIndex.reset();
    }
    // sorting blocks
    // rearranging cables
    // some animation trigger

private:
    std::array<OperatorBlock, 4> op;
    std::array<PatchCable, 16> cable;
    std::optional<int> currentCableIndex;
};

