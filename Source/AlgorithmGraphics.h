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
        blockCenterCoords = bounds.getCentre();
        
        float blockSize = bounds.getWidth() * 0.65f;
        
        blockCenterCoords = bounds.getCentre();
        juce::Rectangle blockRectangle = { blockCenterCoords.x - blockSize/2,
            blockCenterCoords.y - blockSize/2,
            blockSize, blockSize
        };

        juce::Path frontPath, leftSidePath, rightSidePath, botSidePath, topSidePath;
        
        calculatePerspective(blockRectangle);
        
        leftSidePath = createSidePath(blockRectangle.getTopLeft(), perspectiveTopLeft, perspectiveBotLeft, blockRectangle.getBottomLeft());
        leftSidePath = leftSidePath.createPathWithRoundedCorners(1.0f);
        
        rightSidePath = createSidePath(blockRectangle.getTopRight(), perspectiveTopRight, perspectiveBotRight, blockRectangle.getBottomRight());
        rightSidePath = rightSidePath.createPathWithRoundedCorners(1.0f);

        botSidePath.startNewSubPath(perspectiveBotLeft);
        botSidePath.lineTo(perspectiveBotRight);

        topSidePath.startNewSubPath(perspectiveTopLeft);
        topSidePath.lineTo(perspectiveTopRight);

        g.setColour(juce::Colour(70, 204, 164));
        g.strokePath(leftSidePath, juce::PathStrokeType(2.0f));
        g.strokePath(rightSidePath, juce::PathStrokeType(2.0f));
        g.strokePath(topSidePath, juce::PathStrokeType(2.0f));
        g.strokePath(botSidePath, juce::PathStrokeType(2.0f));

        
        frontPath.addRectangle(blockRectangle);
        frontPath = frontPath.createPathWithRoundedCorners(1.0f);
        g.setColour(juce::Colour(90, 224, 184));
        g.strokePath(frontPath, juce::PathStrokeType(2.0f));
        
        
        
        
        
        
        
        
        
        
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
    
    juce::Point<float> interpolateToVanishing(juce::Point<float> origin)
    {
        
        juce::Point<float> interpolatedPoint {
            origin.x + (vanPoint.x - origin.x) * perspective,
            origin.y + (vanPoint.y - origin.y) * perspective };
        
        return interpolatedPoint;
    }
    
    void calculatePerspective(juce::Rectangle<float> block)
    {
        perspectiveTopLeft = interpolateToVanishing(block.getTopLeft());
        perspectiveTopRight = interpolateToVanishing(block.getTopRight());
        perspectiveBotLeft = interpolateToVanishing(block.getBottomLeft());
        perspectiveBotRight = interpolateToVanishing(block.getBottomRight());
    }

private:
    float width, x, y;
    float perspective = 0.5f;
    juce::Point<float> vanPoint,
    blockCenterCoords,
    perspectiveTopLeft,
    perspectiveTopRight,
    perspectiveBotLeft,
    perspectiveBotRight;
    
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
        
        // draw background fill
        juce::Path boundsPath;
        boundsPath.addRoundedRectangle(bounds, 10, 10);
        g.setColour(juce::Colour(40, 42, 41));
        g.fillPath(boundsPath);
        g.setColour(juce::Colour(30, 32, 31));
        g.strokePath(boundsPath, juce::PathStrokeType(2.0f));

        
        float x = bounds.getX();
        float y = bounds.getY();
        // persepctive
        juce::Path perspectivePath;
        perspectivePath.startNewSubPath(x, y);
        perspectivePath.lineTo(x + bounds.getWidth(), y + bounds.getHeight());
        perspectivePath.startNewSubPath(x + bounds.getWidth(), y);
        perspectivePath.lineTo(x, y + bounds.getHeight());
        g.setColour(juce::Colour(90, 90, 90));
        g.strokePath(perspectivePath, juce::PathStrokeType(2));

        
        float blockIncr = bounds.getWidth()/4.0f;

        juce::Point<float> vp = bounds.getCentre();
        op[0].setVanishingPoint(op[0].getLocalPoint(this, vp), 0.1f);
        op[1].setVanishingPoint(op[1].getLocalPoint(this, vp), 0.1f);
        op[2].setVanishingPoint(op[2].getLocalPoint(this, vp), 0.1f);
        op[3].setVanishingPoint(op[3].getLocalPoint(this, vp), 0.1f);

        
    }
    
    void resized() override
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
            
        /*
            if (dragging box)
            {
                snap into place?
                check for collision and skip
         
            }
            if dragging cable
            {
                each op keeps count of input/output
                
         
            }
         
         
         
         
         */
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

