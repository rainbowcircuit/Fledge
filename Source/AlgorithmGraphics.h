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
    PatchCable()
    {
        isInUse = false;
        isConnected = false;
    }
    
    void setThisCableIndex(int thisCableIndex)
    {
        this->thisCableIndex = thisCableIndex;
    }
    
    void paint(juce::Graphics& g) override
    {
        juce::Path cablePath, cableEndPath;
        g.setColour(juce::Colour(200, 200, 200));

        if (isInUse){
            // draw output arc and path start
            cableEndPath.addCentredArc(outputPoint.x, outputPoint.y, 2, 2, 0.0f, 0.0f, 6.28f, true);
            g.fillPath(cableEndPath);
            cablePath.startNewSubPath(outputPoint);
            
            // testing only
            g.drawText(juce::String(thisCableIndex), outputPoint.x + thisCableIndex * 5.0f, outputPoint.y, 10.0f, 10.0f, juce::Justification::centred);
            
            if (isConnected)
            {
                float dx = inputPoint.x - outputPoint.x;
                float dy = inputPoint.y - outputPoint.y;
                float slack = std::abs(dy) * 0.5f + 20.0f;

                
                juce::Point<float> bezierCoords1(outputPoint.x + dx * 0.15f, outputPoint.y + slack);
                juce::Point<float> bezierCoords2(inputPoint.x - dx * 0.15f, inputPoint.y + 20.0f);

                // draw to input point and its arc
                cablePath.cubicTo(bezierCoords1, bezierCoords2, inputPoint);
                g.strokePath(cablePath, juce::PathStrokeType(1.0));
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
                g.strokePath(cablePath, juce::PathStrokeType(1.0));
                cableEndPath.addCentredArc(mousePoint.x, mousePoint.y, 2, 2, 0.0f, 0.0f, 6.28f, true);
                g.fillPath(cableEndPath);
            }
            g.strokePath(cablePath, juce::PathStrokeType(1.0));
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
    
    bool isCableSelected() { return 1; }
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
    int outputIndex, inputIndex, thisCableIndex;
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
        
        blockSize = bounds.getWidth() * 0.2f;
        
        blockRectangle = { blockCenterCoords.x - blockSize/2,
            blockCenterCoords.y - blockSize/2,
            blockSize, blockSize };

        juce::Path frontPath, leftSidePath, rightSidePath, botSidePath, topSidePath;
        
        calculatePerspective();
        
        drawBlockBackground(g);
        drawBlockPoint(g, blockRectangle.getCentreX(), blockRectangle.getY() - 8.0f);
        drawBlockPoint(g, blockRectangle.getCentreX(), blockRectangle.getY() + blockRectangle.getHeight() + 8.0f);
        drawBlockForeground(g);
        
        g.setColour(juce::Colour(200, 200, 200));
        g.drawText(juce::String(operatorIndex), blockRectangle, juce::Justification::centred);
        
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
        
    std::array<float, 4> getInputIndex()
    {
        return inputIndex;
    }
    
    void setInput(int index, float value) // potentially with bool
    {
        inputIndex[index] = value;
        DBG(inputIndex[0] << inputIndex[1] << inputIndex[2] << inputIndex[3]);
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
            
            for (int j = 0; j < 4; j++){
                addAndMakeVisible(cable[i][j]);
                cable[i][j].setInterceptsMouseClicks(false, false);
                cable[i][j].setThisCableIndex(j);
            }
        }
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        calculateCoordinates(bounds);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        juce::Point<float> vp = bounds.getCentre().toFloat();

        calculateCoordinates(bounds.toFloat());
        
        op[0].setBlockCenter(x + blockIncr * 2, y);
        op[1].setBlockCenter(x + blockIncr, y + blockIncr);
        op[2].setBlockCenter(x + blockIncr * 2, y + blockIncr * 2);
        op[3].setBlockCenter(x + blockIncr * 3, y + blockIncr * 3);

        for (int i = 0; i < 4; i++){
            op[i].setBounds(bounds);
            op[i].setVanishingPoint(vp, 0.1f);
            
            for (int j = 0; j < 4; j++)
            {
                cable[i][j].setBounds(bounds);
            }
        }
    }

    
    void mouseDown(const juce::MouseEvent& m) override
    {
        bool modifier = m.mods.isCommandDown();
        for (int i = 0; i < 4; i++)
        {
            auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
            if (op[i].isOverOutputPoint(mouse) && op[i].getNumCableAvailable() != 0)
            {
                //********** CREATE NEW CABLES **********//
                int cableIndex = op[i].getNumCableAvailable();
                DBG("mousedown new cable index: " << cableIndex);
                auto outputPoint = op[i].getOutputPoint();
                
                cable[i][cableIndex].setOutputPoint(outputPoint);
                cable[i][cableIndex].setMousePoint(mouse);
                cable[i][cableIndex].setIsInUse(true);
                
                currentOutputBlockIndex = i;
                currentCableIndex = cableIndex;
                dragState = 2; // dragging cable
                
            } else if (op[i].isOverBlock(mouse)) {
                op[i].setBlockInFocus(true);
                dragState = 1; // dragging block
                
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
        int blk = *currentOutputBlockIndex;
        int cbl = *currentCableIndex;

        for (int i = 0; i < 4; i++)
        {
            auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
            auto globalMouse = op[i].getLocalPoint(this, m.getPosition());
            
            op[i].setBlockInFocus(false);
            op[i].setPointInFocus(false);


            if (op[i].isOverBlock(mouse) && *dragState == 1)
            {
                // DRAGGING BLOCK
                op[i].setBlockInFocus(true);
                op[i].setBlockCenter(globalMouse.x, globalMouse.y);
                
                // REFRESH CABLE POSITION WITH BLOCK
                for (int j = 0; j < 4; j++)
                {
                    bool inUse = cable[i][j].getIsInUse();
                    bool isConnected = cable[i][j].getIsConnected();
                    int outputIndex = cable[i][j].getCableOutputIndex();
                    int inputIndex = cable[i][j].getCableInputIndex();

                    if (outputIndex == blk && outputIndex != -1 && inUse && isConnected)
                    {
                        auto outputPoint = op[outputIndex].getOutputPoint();
                        cable[i][j].setOutputPoint(outputPoint);
                    }
                    
                    if (inputIndex == i && inputIndex != -1 && inUse && isConnected)
                    {
                        auto inputPoint = op[inputIndex].getInputPoint();
                        cable[i][j].setInputPoint(inputPoint);
                    }
                }
            }
            
            if (currentCableIndex.has_value() && *dragState == 2)
            {
                auto outputPoint = op[blk].getOutputPoint();
                cable[blk][cbl].setIsInUse(true);
                cable[blk][cbl].setOutputPoint(outputPoint);
                cable[blk][cbl].setMousePoint(mouse);
            }
            
            if(cable[blk][cbl].getIsInUse() && cable[blk][cbl].getIsConnected() && *dragState == 2){
                int outputIndex = cable[blk][cbl].getCableOutputIndex();
                auto outputPoint = op[outputIndex].getOutputPoint();
                auto inputPoint = op[i].getInputPoint();
                
                cable[blk][cbl].setOutputPoint(outputPoint);
                cable[blk][cbl].setInputPoint(inputPoint);
            }
            
            if (op[i].isOverOutputPoint(mouse))
            {
                op[i].setPointInFocus(true);
            }
            if (op[i].isOverInputPoint(mouse))
            {
                op[i].setPointInFocus(true);
            }
        }
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        int blk = *currentOutputBlockIndex;
        int cbl = *currentCableIndex;

        for (int i = 0; i < 4; i++)
        {
            auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
            
            if (currentCableIndex.has_value() && op[i].isOverInputPoint(mouse) && *dragState == 2)
            {
                // create new cable
                DBG("cable mouse up on: " << cbl);

                auto inputPoint = op[i].getInputPoint();
                cable[blk][cbl].setInputPoint(inputPoint);
                cable[blk][cbl].setCableInputIndex(i);
                cable[blk][cbl].setCableOutputIndex(blk);
                cable[blk][cbl].setIsInUse(true);
                cable[blk][cbl].setIsConnected(true);
                
                int outputIndex = cable[blk][cbl].getCableOutputIndex();
                op[blk].setNumCableAvailable(-1);

                int dbg = op[outputIndex].getNumCableAvailable();

                DBG("origin op: " << outputIndex << " cables left: " << dbg);
                DBG("dest op point: " << i);

                break;
            }
            
            if (currentCableIndex.has_value() && !op[i].isOverInputPoint(mouse) && *dragState == 2)
            {
                cable[blk][cbl].setIsInUse(false);
                cable[blk][cbl].setIsConnected(false);
            }
        }

        currentCableIndex.reset();
        currentOutputBlockIndex.reset();
        dragState.reset();
        
        for (int i = 0; i < 4; i++)
        {
            op[i].setBlockInFocus(false);
            op[i].setPointInFocus(false);
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
                result |= (1 << (3 - i));
        }
        return result;
    }

    
    juce::Rectangle<float> bounds;
    float x, y, width, widthMargin, height, heightMargin, blockIncr;
    
    
    std::array<OperatorBlock, 4> op;
    std::array<std::array<PatchCable, 4>, 4> cable;
    std::optional<int> currentCableIndex, currentOutputBlockIndex;
    std::optional<int> dragState; // 0 = out of bounds, 1 = dragging block, 2 = dragging cable
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
        drawAlgorithm(g, bounds.getX(), bounds.getY(), bounds.getWidth());
    }
    
    void selectAlgorithm()
    {
        switch(graphicIndex){
            case 0:
                block.blockToUse = { 4, 7, 10, 11 };
                block.connectValue = { DOWN, DOWN, DOWN, DOWN };
                block.label = { "3","2", "1", "N" };
                break;
                
            case 1:
                block.blockToUse = { 6, 9, 10, 11 };
                block.connectValue = { RIGHTDOWN, DOWN, DOWN, DOWN };
                block.label = { "3","1", "2", "N" };
                break;
                
            case 2:
                block.blockToUse = { 7, 8, 9, 10 };
                block.connectValue = { DOWN, DOWNLEFT, DOWN, DOWN };
                block.label = { "3","2", "N", "1" };

                break;
                
            case 3:
                block.blockToUse = { 4, 6, 7, 10 };
                block.connectValue = { DOWN, DOWNRIGHT, DOWN, DOWN };
                block.label = { "3","N", "2", "1" };
                break;

            case 4:
                block.blockToUse = { 6, 7, 10, 11 };
                block.connectValue = { DOWNRIGHT, RIGHTDOWN, DOWN, DOWN };
                block.label = { "N","3", "1", "2" };
                break;
                
            case 5:
                block.blockToUse = { 6, 7, 8, 10 };
                block.connectValue = { DOWNRIGHT, DOWN, DOWNLEFT, DOWN };
                block.label = { "N","3", "2", "1" };
                break;

            case 6:
                block.blockToUse = { 3, 5, 7, 10 };
                block.connectValue = { DOWNRIGHT, DOWNLEFT, DOWN, DOWN };
                block.label = { "N","3", "2", "1" };
                break;

            case 7:
                block.blockToUse = { 4, 7, 10, 11 };
                block.connectValue = { DOWN, DOWN, DOWN, DOWN };
                block.label = { "N","3", "1", "2" };
                break;

            case 8:
                block.blockToUse = { 4, 7, 8, 10 };
                block.connectValue = { DOWN, DOWN, DOWNLEFT, DOWN };
                block.label = { "N","3", "2", "1" };
                break;
        }

    }
    
    
    void drawAlgorithm(juce::Graphics& g, float x, float y, float size)
    {
        float graphicSize = size * 0.9f;
        float margin = size * 0.15;
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
                    g.setColour(juce::Colour(30, 154, 114));
                    g.strokePath(blockPath, juce::PathStrokeType(1.0f));

                } else {
                    g.setColour(juce::Colour(90, 224, 184));
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
                        
                    } else if (block.connectValue[j] == NONE) {
                        
                    }
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
                g.setColour(juce::Colour(150, 150, 150));
                g.setFont(11.0f);
                
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
        std::array<juce::String, 4> label = { "3","2", "1", "4" };
    };
    enum blockConnect { NONE, DOWN, DOWNLEFT, DOWNRIGHT, LEFTDOWN, RIGHTDOWN };
    blockValues block;
    
};
