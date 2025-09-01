/*
  ==============================================================================

    AlgorithmGraphics.cpp
    Created: 5 Jul 2025 11:37:40am
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "AlgorithmGraphics.h"

PatchCable::PatchCable()
{
    isInUse = false;
    isConnected = false;
}

void PatchCable::paint(juce::Graphics& g)
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
    
void PatchCable::setOutputPoint(juce::Point<float> point)
{
    outputPoint = point;
    repaint();
}

void PatchCable::setInputPoint(juce::Point<float> point)
{
    inputPoint = point;
    repaint();
}

void PatchCable::setMousePoint(juce::Point<float> point)
{
    mousePoint = point;
    repaint();
}

void PatchCable::setIsCableSelected(bool isSelected)
{
    this->isSelected = isSelected;
}

bool PatchCable::getIsMouseOver(juce::Point<float> mouse)
{
    return cableSelectPath.contains(mouse, 3.0f);
}

bool PatchCable::getIsCableSelected()
{
    return isSelected;
}

void PatchCable::setIsCableHoveredOn(bool isHoveredOn)
{
    this->isHoveredOn = isHoveredOn;
}

void PatchCable::setIsConnected(bool isConnected)
{
    this->isConnected = isConnected;
}

void PatchCable::setIsInUse(bool isInUse)
{
    this->isInUse = isInUse;
}

bool PatchCable::getIsInUse()
{
    return isInUse;
}

bool PatchCable::getIsConnected()
{
    return isConnected;
}

void PatchCable::setCableOutputIndex(int outputIndex)
{
    this->outputIndex = outputIndex;
}

int PatchCable::getCableOutputIndex()
{
    return outputIndex;
}

void PatchCable::setCableInputIndex(int inputIndex)
{
    this->inputIndex = inputIndex;
}

int PatchCable::getCableInputIndex()
{
    return inputIndex;
}


void OperatorBlock::setIsOutput(bool isOutput)
{
    this->isOutput = isOutput;
}

void OperatorBlock::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    bounds.reduce(5, 5);
    
    blockSize = bounds.getWidth() * 0.15f;
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
        g.setColour(Colors::cableColor);
        g.drawText(juce::String(operatorIndex + 1), blockRectangle, juce::Justification::centred);

    } else {
        drawBlockPoint(g, blockRectangle.getCentreX(), blockRectangle.getY() - 8.0f);
        g.setColour(Colors::cableColor);
        g.setFont(juce::FontOptions(12.0f));
        g.drawText("Output", blockRectangle.getX(), blockRectangle.getY() - 16.0f, blockRectangle.getWidth(), blockRectangle.getHeight(), juce::Justification::centred);
    }
}

juce::Point<float> OperatorBlock::interpolateToVanishing(juce::Point<float> origin)
{
    
    juce::Point<float> interpolatedPoint {
        origin.x + (vanPoint.x - origin.x) * perspective,
        origin.y + (vanPoint.y - origin.y) * perspective };
    
    return interpolatedPoint;
}

void OperatorBlock::calculatePerspective()
{
    perspectiveTopLeft = interpolateToVanishing(blockRectangle.getTopLeft());
    perspectiveTopRight = interpolateToVanishing(blockRectangle.getTopRight());
    perspectiveBotLeft = interpolateToVanishing(blockRectangle.getBottomLeft());
    perspectiveBotRight = interpolateToVanishing(blockRectangle.getBottomRight());
}


juce::Path OperatorBlock::createSidePath(juce::Point<float> point1, juce::Point<float> point2, juce::Point<float> point3, juce::Point<float> point4)
{
    juce::Path path;
    path.startNewSubPath(point1);
    path.lineTo(point2);
    path.lineTo(point3);
    path.lineTo(point4);
    path.closeSubPath();
    return path;
}

void OperatorBlock::drawBlockPoint(juce::Graphics &g, float x, float y)
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

void OperatorBlock::drawBlockBackground(juce::Graphics &g)
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

void OperatorBlock::drawBlockForeground(juce::Graphics &g)
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
        g.setColour(Colors::mainColors[operatorIndex].withAlpha((float)opacity));
        g.fillPath(graphicPath);
    }
}


void OperatorBlock::setVanishingPoint(juce::Point<float> vanPoint, float perspective)
{
    this->perspective = perspective;
    this->vanPoint = vanPoint;
    repaint();
}

void OperatorBlock::drawPatchPoint(juce::Graphics& g, float x, float y)
{
    juce::Path graphicPath;
    graphicPath.addCentredArc(x, y, 4.0f, 4.0f, 0.0f, 0.0f, 6.28f, true);
    g.setColour(Colors::mainColors[4]);
    g.strokePath(graphicPath, juce::PathStrokeType(1.0f));
}
    
void OperatorBlock::setBlockCenter(float x, float y)
{
    blockCenterCoords.x = x;
    blockCenterCoords.y = y;
    
    // refresh source block
    blockRectangle = { blockCenterCoords.x - blockSize/2,
        blockCenterCoords.y - blockSize/2,
        blockSize, blockSize };
    
    repaint();
}

juce::Point<float> OperatorBlock::getBlockCenter()
{
    return blockCenterCoords;
}

bool OperatorBlock::OperatorBlock::isOverBlock(juce::Point<float> mouse)
{
    // probably should encapsulate this
    juce::Rectangle blockRectangle = { blockCenterCoords.x - blockSize/2,
        blockCenterCoords.y - blockSize/2,
        blockSize, blockSize };
    
    if (!isOutput){ return blockRectangle.contains(mouse); }
    else { return false; }
}

bool OperatorBlock::isOverOutputPoint(juce::Point<float> mouse)
{
    float pointArea = 10.0f;
            
    juce::Rectangle outputPoint(blockRectangle.getCentreX() - pointArea/2, blockRectangle.getY() + blockRectangle.getHeight() - pointArea/2 + 8.0f, pointArea, pointArea);
    
    return outputPoint.contains(mouse);
}

bool OperatorBlock::isOverInputPoint(juce::Point<float> mouse)
{
    float pointArea = 10.0f;
    juce::Rectangle inputPoint(blockRectangle.getCentreX() - pointArea/2, blockRectangle.getY() - pointArea/2 - 8.0f, pointArea, pointArea);
    
    return inputPoint.contains(mouse);
}

juce::Point<float> OperatorBlock::getInputPoint()
{
    juce::Point<float> point(blockRectangle.getCentreX(), blockRectangle.getY() - 8.0f);
    return point;
}

juce::Point<float> OperatorBlock::getOutputPoint()
{
    juce::Point<float> point(blockRectangle.getCentreX(), blockRectangle.getY() + blockRectangle.getHeight() + 8.0f);
    return point;
}


void OperatorBlock::setBlockInFocus(bool focus)
{
    blockInFocus = focus;
}

void OperatorBlock::setPointInFocus(bool focus)
{
    pointInFocus = focus;
}

std::array<float, 4>& OperatorBlock::getInputIndex()
{
    return inputIndex;
}

void OperatorBlock::setInput(int index, float value) // potentially with bool
{
    inputIndex[index] = value;
}

void OperatorBlock::setInput(std::array<float, 4> inputIndex)
{
    this->inputIndex = inputIndex;
}

void OperatorBlock::setInput(int integerIndex)
{
    inputIndex = toBinary4(integerIndex);
}

int OperatorBlock::getOperatorIndex()
{
    return operatorIndex;
}

void OperatorBlock::setOperatorIndex(int operatorIndex)
{
    this->operatorIndex = operatorIndex;
}

void OperatorBlock::setNumCableAvailable(int amount)
{
    numCableAvailable += amount;
    
    if (numCableAvailable >= 4) {
        numCableAvailable = 4;
    } else if (numCableAvailable <= 0) {
        numCableAvailable = 0;
    }
}

int OperatorBlock::getNumCableAvailable()
{
    return numCableAvailable;
}


AlgorithmGraphics::AlgorithmGraphics(FledgeAudioProcessor& p) : audioProcessor(p)
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
        op[i].toBack();

        
        for (int j = 0; j < 4; j++){
            addAndMakeVisible(cable[i][j]);
            cable[i][j].setInterceptsMouseClicks(false, false);
            cable[i][j].setAlwaysOnTop(true);
            cable[i][j].toFront(false);
        }
    }
    setGUIFromParameter();
    addAndMakeVisible(clearCablesButton);
    clearCablesButton.addListener(this);
    
    setWantsKeyboardFocus(true);
    startTimerHz(60);
}

AlgorithmGraphics::~AlgorithmGraphics()
{
    clearCablesButton.removeListener(this);
}

void AlgorithmGraphics::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    fillControlPanel(g, bounds);
    calculateCoordinates(bounds);
    
    op[0].setVanishingPoint(vp, 0.1f);
    op[1].setVanishingPoint(vp, 0.1f);
    op[2].setVanishingPoint(vp, 0.1f);
    op[3].setVanishingPoint(vp, 0.1f);
    
    drawGridBox(g, bounds);
}

void AlgorithmGraphics::resized()
{
    auto bounds = getLocalBounds();
    juce::Point<float> vp = bounds.getCentre().toFloat();
    calculateCoordinates(bounds.toFloat());
    

    op[4].setBlockCenter(x + widthMargin + width/2, y + height * 1.05f); // output
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

void AlgorithmGraphics::drawGridBox(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    juce::Rectangle<float> frontRectangle = bounds;
    float reduceAmount = bounds.getWidth() * 0.0525f;
    frontRectangle.reduce(reduceAmount, reduceAmount);
    
    juce::Path backPath, frontPath, sidePath;
    float backBoxWidth = frontRectangle.getWidth() * 0.85f;
    float backBoxHeight = frontRectangle.getHeight() * 0.85f;
    g.setColour(juce::Colour(45, 47, 46));

    float maxY = frontRectangle.getBottom() - backBoxHeight;
    float constrainedY = juce::jlimit(frontRectangle.getY(), maxY, vp.y - backBoxHeight / 2);
    float constrainedX = juce::jlimit(frontRectangle.getX(),
                                       frontRectangle.getRight() - backBoxWidth,
                                       vp.x - backBoxWidth / 2);

    juce::Rectangle<float> backRectangle = { constrainedX, constrainedY, backBoxWidth, backBoxHeight };

    backPath.addRoundedRectangle(backRectangle, 1.0f);
    g.strokePath(backPath, juce::PathStrokeType(0.75f));
    
    frontPath.addRoundedRectangle(frontRectangle, 1.0f);
    g.strokePath(frontPath, juce::PathStrokeType(0.75f));
    
    juce::Path gridWidthPath, gridHeightPath;
    for (int i = 0; i <= 10; i++) {
        for (int j = 0; j < 10; j++) {
            float frontHeightIncr = (frontRectangle.getHeight()/10) * i;
            float backHeightIncr = (backRectangle.getHeight()/10) * i;
            float frontWidthIncr = (frontRectangle.getWidth()/10) * j;
            float backWidthIncr = (backRectangle.getWidth()/10) * j;

            gridHeightPath.startNewSubPath(frontRectangle.getTopLeft().x,
                                     frontRectangle.getTopLeft().y + frontHeightIncr);
            gridHeightPath.lineTo(backRectangle.getTopLeft().x,
                            backRectangle.getTopLeft().y + backHeightIncr);
            gridHeightPath.lineTo(backRectangle.getTopRight().x,
                            backRectangle.getTopRight().y + backHeightIncr);
            gridHeightPath.lineTo(frontRectangle.getTopRight().x,
                            frontRectangle.getTopRight().y + frontHeightIncr);

            gridWidthPath.startNewSubPath(frontRectangle.getTopLeft().x + frontWidthIncr,
                                     frontRectangle.getTopLeft().y);
            gridWidthPath.lineTo(backRectangle.getTopLeft().x + backWidthIncr,
                            backRectangle.getTopLeft().y);
            gridWidthPath.lineTo(backRectangle.getBottomLeft().x + backWidthIncr,
                            backRectangle.getBottomLeft().y);
            gridWidthPath.lineTo(frontRectangle.getBottomLeft().x + frontWidthIncr,
                            frontRectangle.getBottomLeft().y);

            g.strokePath(gridHeightPath, juce::PathStrokeType(0.75f));
            g.strokePath(gridWidthPath, juce::PathStrokeType(0.75f));
        }
    }
}

void AlgorithmGraphics::averageVanishingPoint()
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

void AlgorithmGraphics::setFromAlgorithmSelection(int selectionIndex)
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

void AlgorithmGraphics::buttonClicked(juce::Button* button)
{
    if (button == &clearCablesButton)
    {
        deleteAllCables();
        clearAllInputs();
    }
}


void AlgorithmGraphics::mouseDown(const juce::MouseEvent& m)
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

void AlgorithmGraphics::mouseDrag(const juce::MouseEvent& m)
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

void AlgorithmGraphics::mouseUp(const juce::MouseEvent& m)
{
    int blk = *currentOutputBlockIndex;
    int cbl = *currentCableIndex;

    for (int i = 0; i <= 4; i++)
    {
        auto mouse = m.getEventRelativeTo(&op[i]).getPosition().toFloat();
        if (currentCableIndex.has_value() && op[i].isOverInputPoint(mouse) && *dragState == 2)
        {
            
            op[i].setInput(blk, 1.0f);
            setCable(blk, cbl, i);
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

void AlgorithmGraphics::moveBlock(int blockToMove, juce::Point<float> newPoint)
{
    op[blockToMove].setBlockCenter(newPoint.x, newPoint.y);
    saveBlockPosition();
}

void AlgorithmGraphics::saveBlockPosition()
{
    if (auto* processor = dynamic_cast<FledgeAudioProcessor*>(&audioProcessor))
    {
        processor->saveBlockPosition(op[0].getBlockCenter(),
                                     op[1].getBlockCenter(),
                                     op[2].getBlockCenter(),
                                     op[3].getBlockCenter());
    }
}

void AlgorithmGraphics::followCable(int blockToMove)
{
    auto outputPoint = op[blockToMove].getOutputPoint();
    setCableOutputCoords(blockToMove, outputPoint);
    
    auto inputPoint = op[blockToMove].getInputPoint();
    setCableInputCoords(blockToMove, inputPoint);
}

void AlgorithmGraphics::startNewCableOnMouseDown(int originBlock, int cableIndex, juce::Point<float> mouse)
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

void AlgorithmGraphics::setCable(int originBlock, int cableIndex, int destBlock)
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

void AlgorithmGraphics::setCableOutputCoords(int originBlock, juce::Point<float> newOutputPoint)
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

void AlgorithmGraphics::setCableInputCoords(int destinationBlock, juce::Point<float> newInputPoint)
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

juce::Point<float> AlgorithmGraphics::limitBlockDrag(juce::Point<float> mousePoint)
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

void AlgorithmGraphics::connectCableToBlock(int originBlock, int cableToConnect, int destinationBlock)
{
    
}

void AlgorithmGraphics::disconnectCableFromBlock(int cableToDisconnect, int disconnectedBlock)
{
    
}

void AlgorithmGraphics::setGUIFromParameter()
{
}

void AlgorithmGraphics::setParameterFromGUI()
{
    // set output
    for (int i = 0; i <= 4; i++){
        routingAtomic[i].store(fromBinary4(op[i].getInputIndex()));
    }
    triggerAsyncUpdate();
}
    
void AlgorithmGraphics::handleAsyncUpdate()
{
    auto outputParam = audioProcessor.params->apvts.getParameter("outputRouting");
    if (outputParam != nullptr){
        float outputGainFloat = outputParam->convertTo0to1(routingAtomic[4].load());
        outputParam->setValueNotifyingHost(outputGainFloat);
        
    } else {
        DBG("Parameter not found for output routing");
    }
    
    // set operators
    for (int i = 0; i < 4; i++) {
        auto operatorParam = audioProcessor.params->apvts.getParameter("operator" + juce::String(i) + "Routing");

        if (operatorParam != nullptr){
            float operatorGainFloat = operatorParam->convertTo0to1(routingAtomic[i].load());
            operatorParam->setValueNotifyingHost(operatorGainFloat);
        } else {
            DBG("Parameter not found for index: " << i);
        }
    }
}

void AlgorithmGraphics::selectCable(juce::Point<float> mouse)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            bool cableSelected = cable[i][j].getIsMouseOver(mouse);
            cable[i][j].setIsCableSelected(cableSelected);
        }
    }
}

void AlgorithmGraphics::deleteAllCables()
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

void AlgorithmGraphics::clearAllInputs()
{
    for (int i = 0; i <= 4; i++) {
            op[i].setInput(0);
    }
}

void AlgorithmGraphics::deleteSelectedCable()
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
                DBG("before: " << blockInputIndex[0] << blockInputIndex[1] << blockInputIndex[2] << blockInputIndex[3]);

                blockInputIndex[outputIndex] = 0.0f;
                DBG("after: " << blockInputIndex[0] << blockInputIndex[1] << blockInputIndex[2] << blockInputIndex[3]);

                int modifiedInputIndex = fromBinary4(blockInputIndex);
                op[inputIndex].setInput(modifiedInputIndex);
                cable[i][j].setCableOutputIndex(-1);
                
            }
        }
    }
}

void AlgorithmGraphics::calculateCoordinates(juce::Rectangle<float> bounds)
{
    x = bounds.getX();
    y = bounds.getY();
    widthMargin = bounds.getWidth() * 0.05f;
    heightMargin = bounds.getHeight() * 0.05f;
    height = bounds.getHeight() * 0.9f;
    width = bounds.getWidth() * 0.9f;
    blockIncr = width * 0.15f;
}
    
void AlgorithmGraphics::timerCallback()
{
    auto mouse = getMouseXYRelative().toFloat();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            
            // hovering on cable
            cable[i][j].setIsCableHoveredOn(cable[i][j].getIsMouseOver(mouse));
            
        }
    }
}

bool AlgorithmGraphics::keyPressed(const juce::KeyPress& key)
{
    if (key.getKeyCode() == juce::KeyPress::backspaceKey)
    {
        deleteSelectedCable();
    //    clearAllInputs();
        setParameterFromGUI();
    }
    return true;
}


