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
    inline std::array<juce::Colour, 5> mainColors {
    juce::Colour(47, 222, 227),
    juce::Colour(64, 229, 210),
    juce::Colour(57, 229, 192),
    juce::Colour(69, 227, 179),
    juce::Colour(241, 241, 241),
    };

    inline std::array<juce::Colour, 5> mainHoverColors {
        juce::Colour(57, 232, 237),
        juce::Colour(74, 239, 220),
        juce::Colour(67, 239, 202),
        juce::Colour(79, 237, 189),
        juce::Colour(251, 251, 251),
    };
};
