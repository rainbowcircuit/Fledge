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

public:
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        bounds.reduce(5, 5);
        
        blockSize = bounds.getWidth() * 0.15f;
        
        blockRectangle = { blockCenterCoords.x - blockSize/2,
            blockCenterCoords.y - blockSize/2,
            blockSize, blockSize
        };

        juce::Path frontPath, leftSidePath, rightSidePath, botSidePath, topSidePath;
        
        calculatePerspective();
        
        drawBlockBackground(g);
        drawBlockPoint(g, blockRectangle.getCentreX(), blockRectangle.getY() - 8.0f);
        drawBlockPoint(g, blockRectangle.getCentreX(), blockRectangle.getY() + blockRectangle.getHeight() + 8.0f);
        drawBlockForeground(g);
        
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
        
        pointPath.addCentredArc(x, y, 2.0f, 2.0f, 0.0f, 0.0f, 6.28f, true);
        g.setColour(juce::Colour(70, 204, 164));
        g.fillPath(pointPath);
        
        if (pointInFocus)
        {
            outlinePath.addCentredArc(x, y, 6.0f, 6.0f, 0.0f, 0.0f, 6.28f, true);
            g.setColour(juce::Colour(50, 184, 144));
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

        g.setColour(juce::Colour(30, 154, 114));
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
        
        g.setColour(juce::Colour(90, 224, 184));
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
        g.setColour(juce::Colour(120, 120, 120));
        g.strokePath(graphicPath, juce::PathStrokeType(1.0f));
    }
    
    //******* Sets interaction with mouse
    
    void setBlockCenter(float x, float y)
    {
        blockCenterCoords.x = x;
        blockCenterCoords.y = y;
        repaint();
    }

    bool isOverBlock(juce::Point<float> mouse)
    {
        // probably should encapsulate this
        juce::Rectangle blockRectangle = { blockCenterCoords.x - blockSize/2,
            blockCenterCoords.y - blockSize/2,
            blockSize, blockSize };

        return blockRectangle.contains(mouse);
    }
    
    bool isOverPoint(juce::Point<float> mouse)
    {
        float blockSize = width * 0.9f;
        float blockMargin = width * 0.05f;
        float pointArea = 10.0f;
        
        juce::Rectangle inputPoint(blockRectangle.getCentreX() - pointArea/2, blockRectangle.getY() - pointArea/2 - 8.0f, pointArea, pointArea);
        
        juce::Rectangle outputPoint(blockRectangle.getCentreX() - pointArea/2, blockRectangle.getY() + blockRectangle.getHeight() - pointArea/2 + 8.0f, pointArea, pointArea);
        
        bool inputBool = inputPoint.contains(mouse);
        bool outputBool = outputPoint.contains(mouse);
        
        return inputBool || outputBool;
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
    
    
    
    
    //
    
    
    std::array<float, 4> getInputIndex()
    {
        return inputIndex;
    }
    
    void setInput(int index, float value) // potentially with bool
    {
        inputIndex[index] = value;
        
    }

    int getOperatorIndex()
    {
        return operatorIndex;
    }

    void setOperatorIndex(int operatorIndex)
    {
        this->operatorIndex = operatorIndex;
    }

private:
    float width, x, y;
    
    int operatorIndex;
    std::array<float, 4> inputIndex;
    
    // drawing state
    bool blockInFocus;
    bool pointInFocus;
    
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



class AlgorithmGraphics : public juce::Component
{
public:
    
    AlgorithmGraphics()
    {
        for (int i = 0; i < 4; i++)
        {
            addAndMakeVisible(op[i]);
            op[i].setInterceptsMouseClicks(false, false);
            op[i].setOperatorIndex(i);
        }
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        calculateCoordinates(bounds);
        
        // draw background fill
        juce::Path boundsPath;
        boundsPath.addRoundedRectangle(bounds, 10, 10);
        g.setColour(juce::Colour(40, 42, 41));
        g.fillPath(boundsPath);
        g.setColour(juce::Colour(30, 32, 31));
        g.strokePath(boundsPath, juce::PathStrokeType(2.0f));

        // persepctive
        juce::Path perspectivePath;
        perspectivePath.startNewSubPath(x, y);
        perspectivePath.lineTo(x + bounds.getWidth(), y + bounds.getHeight());
        perspectivePath.startNewSubPath(x + bounds.getWidth(), y);
        perspectivePath.lineTo(x, y + bounds.getHeight());
        g.setColour(juce::Colour(90, 90, 90));
        g.strokePath(perspectivePath, juce::PathStrokeType(2));

        
        juce::Point<float> vp = bounds.getCentre();
        op[0].setVanishingPoint(vp, 0.1f);
        op[1].setVanishingPoint(vp, 0.1f);
        op[2].setVanishingPoint(vp, 0.1f);
        op[3].setVanishingPoint(vp, 0.1f);
        

    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();

        calculateCoordinates(bounds.toFloat());

        op[0].setBounds(bounds);
        op[1].setBounds(bounds);
        op[2].setBounds(bounds);
        op[3].setBounds(bounds);
        
        op[0].setBlockCenter(x, y);
        op[1].setBlockCenter(x + blockIncr, y + blockIncr);
        op[2].setBlockCenter(x + blockIncr * 2, y + blockIncr * 2);
        op[3].setBlockCenter(x + blockIncr * 3, y + blockIncr * 3);
    }

    void mouseDown(const juce::MouseEvent& m) override
    {
        bool modifier = m.mods.isCommandDown();
        for (int i = 0; i < 4; i++)
        {
            auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
            if (op[i].isOverPoint(mouse))
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
        for (int i = 0; i < 4; i++)
        {
            auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
            auto globalMouse = op[i].getLocalPoint(this, m.getPosition());
            
            op[i].setBlockInFocus(false);
            op[i].setPointInFocus(false);

            if (op[i].isOverBlock(mouse))
            {
                op[i].setBlockInFocus(true);
                op[i].setBlockCenter(globalMouse.x, globalMouse.y);
            }
            if (op[i].isOverPoint(mouse))
            {
                op[i].setPointInFocus(true);
            }
        }
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        currentCableIndex.reset();
        for (int i = 0; i < 4; i++)
        {
            op[i].setBlockInFocus(false);
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
        blockIncr = width * 0.25f;
    }
    
    float getSnappedPosition(float coords)
    {
        int roundedCoords = coords/blockIncr;
      //  return roundedCoords * blockIncr;
        return coords;
    }
    
private:
    juce::Rectangle<float> bounds;
    float x, y, width, widthMargin, height, heightMargin, blockIncr;
    
    
    std::array<OperatorBlock, 4> op;
    std::array<PatchCable, 16> cable;
    std::optional<int> currentCableIndex;
};

