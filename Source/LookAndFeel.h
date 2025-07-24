/*
  ==============================================================================

    LookAndFeel.h
    Created: 30 May 2025 11:28:06am
    Author:  Takuma Matsui

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once

namespace Colors
{
    inline juce::Colour textColor = juce::Colour(130, 130, 130);

    inline std::array<juce::Colour, 5> mainColors {
        juce::Colour(47, 222, 227),
        juce::Colour(64, 229, 210),
        juce::Colour(57, 229, 192),
        juce::Colour(69, 227, 179),
        juce::Colour(241, 241, 241),
    };

    inline std::array<juce::Colour, 5> mainHoverColors {
        juce::Colour(67, 242, 247),
        juce::Colour(84, 249, 230),
        juce::Colour(77, 249, 212),
        juce::Colour(89, 247, 199),
        juce::Colour(251, 251, 251),
    };

    // Add LevelMeter colors
    namespace LevelMeter
    {
        inline juce::Colour background = juce::Colour(30, 30, 30);     // Dark background
        inline juce::Colour levelOK = juce::Colour(69, 227, 179);     // Green for normal levels (using one of your main colors)
        inline juce::Colour tooLoud = juce::Colour(255, 80, 80);      // Red for levels above 0dB
    }
};