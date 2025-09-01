/*
  ==============================================================================

    Parameters.h
    Created: 7 Aug 2025 7:42:26pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FledgeAudioProcessor;
class ParameterInstance;

class Parameters
{
public:
    Parameters(FledgeAudioProcessor& p);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
private:
    FledgeAudioProcessor& audioProcessor;
    
public:
    juce::AudioProcessorValueTreeState apvts;
    
    std::array<std::unique_ptr<ParameterInstance>, 4>
    attack,
    decay,
    sustain,
    release,
    ratio,
    amplitude,
    phase,
    routing;
    
    std::unique_ptr<ParameterInstance>
    pitchModInit,
    pitchModAttack,
    pitchModPeaking,
    pitchModDecay,
    pitchModSustain,
    pitchModRelease,
    voiceCount,
    voiceSlew,
    globalAttack,
    globalDecay,
    globalSustain,
    globalRelease,
    globalModIndex,
    outputRouting;
    
};


class ParameterInstance : public juce::AudioProcessorParameter::Listener, juce::AsyncUpdater
{
public:
    
    ParameterInstance(FledgeAudioProcessor& p, Parameters& pm, juce::String paramID);
    
    //==============================================================================
    void parameterValueChanged (int /*maybe unused*/, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    void handleAsyncUpdate() override;
    void triggerUpdate();
    
    //==============================================================================
    float get() const noexcept;
    float getSafe() const noexcept;

private:
    float valueSafe;
    std::atomic<float> value;
    std::atomic<float> cachedValue;

    juce::String paramID;
    juce::RangedAudioParameter* rangedParam = nullptr;

    FledgeAudioProcessor& audioProcessor;
    Parameters& param;
};
