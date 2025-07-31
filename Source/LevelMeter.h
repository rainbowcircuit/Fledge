/*
  ==============================================================================

    LevelMeter.h
    Created: 31 Jul 2025 11:04:32am
    Author:  Ryan Page

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Measurement.h"

class LevelMeter  : public juce::Component, private juce::Timer
{
public:
    LevelMeter(Measurement& measurementL, Measurement& measurementR);
    ~LevelMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    int positionForLevel(float dbLevel) const noexcept
    {
        // Fixed: Map higher dB levels to higher x positions (left to right)
        return int(std::round(juce::jmap(dbLevel, mindB, maxdB, maxPos, minPos)));
    }

    void drawLevel(juce::Graphics& g, float level, int y, int height);
    void updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const;

    Measurement& measurementL;
    Measurement& measurementR;

    static constexpr float maxdB = 6.0f;
    static constexpr float mindB = -60.0f;

    float maxPos = 0.0f;
    float minPos = 0.0f;

    static constexpr float clampdB = -120.0f;
    static constexpr float clampLevel = 0.000001f;  // -120 dB

    float dbLevelL;
    float dbLevelR;

    static constexpr int refreshRate = 60;

    float decay = 0.0f;
    float levelL = clampLevel;
    float levelR = clampLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};
