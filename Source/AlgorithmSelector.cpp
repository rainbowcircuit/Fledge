
#include "AlgorithmSelector.h"

AlgorithmSelectInterface::AlgorithmSelectInterface(FledgeAudioProcessor& p, AlgorithmGraphics& a) : algoGraphics(a), audioProcessor(p)
{
    for(int i = 0; i < 8; i++)
    {
        addAndMakeVisible(algorithm[i]);
        algorithmGraphics[i].setIndex(i);
        algorithm[i].setLookAndFeel(&algorithmGraphics[i]);
        algorithm[i].addListener(this);
    }
}
    
AlgorithmSelectInterface::~AlgorithmSelectInterface()
{
    for(int i = 0; i < 8; i++)
    {
        algorithm[i].setLookAndFeel(nullptr);
        algorithm[i].removeListener(this);
    }
}
    
void AlgorithmSelectInterface::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    
    float width = bounds.getWidth() * 0.95f;
    float height = bounds.getHeight() * 0.8f;
    float widthMargin = bounds.getWidth() * 0.025f;
    float heightMargin = bounds.getHeight() * 0.1f;

    float blockWidth = width * 0.25f;
    float blockHeight = height * 0.5f;

    for(int i = 0; i < 8; i++)
    {
        algorithm[i].setBounds(x + widthMargin + blockWidth * (i % 4),
                               y + heightMargin + blockWidth * (i / 4),
                               blockHeight,
                               blockHeight);
    }
}
    

void AlgorithmSelectInterface::buttonClicked(juce::Button* button)
{
    algoGraphics.clearAllInputs();
    if (button == &algorithm[0]){
        setOperatorParam(3, 1);
        setOperatorParam(2, 8);
        setOperatorParam(1, 4);
        setOperatorParam(0, 2);
        setOutputParam(1);
        algoGraphics.setFromAlgorithmSelection(0);
        
    } else if (button == &algorithm[1]) {
        setOperatorParam(3, 0);
        setOperatorParam(2, 8);
        setOperatorParam(1, 0);
        setOperatorParam(0, 6);
        setOutputParam(1);
        algoGraphics.setFromAlgorithmSelection(1);
        
    } else if (button == &algorithm[2]) {
        setOperatorParam(3, 0);
        setOperatorParam(2, 0);
        setOperatorParam(1, 0);
        setOperatorParam(0, 14);
        setOutputParam(1);
        algoGraphics.setFromAlgorithmSelection(2);

    } else if (button == &algorithm[3]) {
        setOperatorParam(3, 0);
        setOperatorParam(2, 8);
        setOperatorParam(1, 8);
        setOperatorParam(0, 6);
        setOutputParam(1);
        algoGraphics.setFromAlgorithmSelection(3);

    } else if (button == &algorithm[4]) {
        setOperatorParam(3, 0);
        setOperatorParam(2, 8);
        setOperatorParam(1, 0);
        setOperatorParam(0, 2);
        setOutputParam(5);
        algoGraphics.setFromAlgorithmSelection(4);

    } else if (button == &algorithm[5]) {
        setOperatorParam(3, 0);
        setOperatorParam(2, 8);
        setOperatorParam(1, 4);
        setOperatorParam(0, 4);
        setOutputParam(3);
        algoGraphics.setFromAlgorithmSelection(5);

    } else if (button == &algorithm[6]) {
        setOperatorParam(3, 0);
        setOperatorParam(2, 8);
        setOperatorParam(1, 0);
        setOperatorParam(0, 0);
        setOutputParam(7);
        algoGraphics.setFromAlgorithmSelection(6);

    } else if (button == &algorithm[7]) {
        setOperatorParam(3, 0);
        setOperatorParam(2, 0);
        setOperatorParam(1, 0);
        setOperatorParam(0, 0);
        setOutputParam(15);
        algoGraphics.setFromAlgorithmSelection(7);

    }
}

void AlgorithmSelectInterface::setOperatorParam(int index, int gainIndex)
{
    auto paramRange = audioProcessor.params->apvts.getParameterRange("operator0Routing");
    float valueScaled = paramRange.convertTo0to1(gainIndex);
    
    juce::String parameterID = "operator" + juce::String(index) + "Routing";
    audioProcessor.params->apvts.getParameter(parameterID)->setValueNotifyingHost(valueScaled);
        
}

void AlgorithmSelectInterface::setOutputParam(int gainIndex)
{
    auto paramRange = audioProcessor.params->apvts.getParameterRange("outputRouting");
    float valueScaled = paramRange.convertTo0to1(gainIndex);
    
    audioProcessor.params->apvts.getParameter("outputRouting")->setValueNotifyingHost(valueScaled);

}

void AlgorithmSelectInterface::setPresetParam(int index)
{
    auto paramRange = audioProcessor.params->apvts.getParameterRange("algorithmPreset");
    float valueScaled = paramRange.convertTo0to1(index);
    
    audioProcessor.params->apvts.getParameter("algorithmPreset")->setValueNotifyingHost(valueScaled);
}

    
void BlockDiagrams::setIndex(int graphicIndex)
{
    this->graphicIndex = graphicIndex;
    selectAlgorithm();
}

void BlockDiagrams::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    drawAlgorithm(g, bounds.getX(), bounds.getY(), bounds.getWidth(), shouldDrawButtonAsDown);
}

void BlockDiagrams::selectAlgorithm()
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


void BlockDiagrams::drawAlgorithm(juce::Graphics& g, float x, float y, float size, bool mouseDown)
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
