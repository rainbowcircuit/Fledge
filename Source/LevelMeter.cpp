#include <JuceHeader.h>
#include "LevelMeter.h"
#include "LookAndFeel.h"

LevelMeter::LevelMeter(Measurement& measurementL_, Measurement& measurementR_)
    : measurementL(measurementL_), measurementR(measurementR_),
      dbLevelL(clampdB), dbLevelR(clampdB)
{
    setOpaque(true);
    startTimerHz(refreshRate);
    decay = 1.0f - std::exp(-1.0f / (float(refreshRate) * 0.2f));
}

LevelMeter::~LevelMeter()
{
}

void LevelMeter::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    // Fill background first
    g.fillAll(juce::Colour(35, 37, 36));

    // Draw levels horizontally - top and bottom channels
    drawLevel(g, dbLevelL, 0, bounds.getHeight() / 2 - 1);  // Top channel
    drawLevel(g, dbLevelR, bounds.getHeight() / 2 + 1, bounds.getHeight() / 2 - 1);  // Bottom channel
}

void LevelMeter::resized()
{
    maxPos = 4.0f;  // Left margin (start position)
    minPos = float(getWidth()) - 4.0f;  // Right margin (end position)
}

void LevelMeter::timerCallback()
{
    updateLevel(measurementL.readAndReset(), levelL, dbLevelL);
    updateLevel(measurementR.readAndReset(), levelR, dbLevelR);

    repaint();
}

void LevelMeter::drawLevel(juce::Graphics& g, float level, int y, int height)
{
    // Fix the position calculation - higher levels should go further right
    int levelPos = positionForLevel(level);
    int zeroPos = positionForLevel(0.0f);  // Position of 0dB mark
    
    if (level > 0.0f) {
        // Level is above 0dB - show clipping
        
        // Draw normal level from start to 0dB in green
        g.setColour(Colors::LevelMeter::levelOK);
        g.fillRect(int(maxPos), y, zeroPos - int(maxPos), height);
        
        // Draw clipping level from 0dB to current level in red
        g.setColour(Colors::LevelMeter::tooLoud);
        g.fillRect(zeroPos, y, levelPos - zeroPos, height);
        
    } else if (levelPos > maxPos) {
        // Level is below 0dB - normal operation
        g.setColour(Colors::LevelMeter::levelOK);
        g.fillRect(int(maxPos), y, levelPos - int(maxPos), height);
    }
    
    // Optional: Draw 0dB marker line
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawVerticalLine(zeroPos, float(y), float(y + height));
}

void LevelMeter::updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const
{
    if (newLevel > smoothedLevel) {
        smoothedLevel = newLevel;  // instantaneous attack
    } else {
        smoothedLevel += (newLevel - smoothedLevel) * decay;
    }
    
    // Convert to dB, but don't clamp too aggressively for clipping detection
    if (smoothedLevel > 0.000001f) {  // Very small threshold to catch more levels
        leveldB = juce::Decibels::gainToDecibels(smoothedLevel);
    } else {
        leveldB = clampdB;
    }
}
