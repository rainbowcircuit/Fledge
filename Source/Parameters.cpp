/*
  ==============================================================================

    Parameters.cpp
    Created: 7 Aug 2025 7:42:26pm
    Author:  Takuma Matsui

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "Parameters.h"

Parameters::Parameters(FledgeAudioProcessor& p) : audioProcessor(p),
apvts(audioProcessor, nullptr, "Parameters", createParameterLayout())
{
    for (int oper = 0; oper < 4; oper++){
        auto incr = juce::String(oper);
        
        attack[oper] = std::make_unique<ParameterInstance>(audioProcessor, *this, "attack" + incr);
        decay[oper] = std::make_unique<ParameterInstance>(audioProcessor, *this, "decay" + incr);
        sustain[oper] = std::make_unique<ParameterInstance>(audioProcessor, *this, "sustain" + incr);
        release[oper] = std::make_unique<ParameterInstance>(audioProcessor, *this, "release" + incr);
        amplitude[oper] = std::make_unique<ParameterInstance>(audioProcessor, *this, "amplitude" + incr);
        ratio[oper] = std::make_unique<ParameterInstance>(audioProcessor, *this, "ratio" + incr);
        phase[oper] = std::make_unique<ParameterInstance>(audioProcessor, *this, "phase" + incr);
        
        juce::String operatorRoutingID = "operator" + juce::String(oper) + "Routing";
        routing[oper] = std::make_unique<ParameterInstance>(audioProcessor, *this, operatorRoutingID);
        
    }
    globalAttack = std::make_unique<ParameterInstance>(audioProcessor, *this, "globalAttack");
    globalDecay = std::make_unique<ParameterInstance>(audioProcessor, *this, "globalDecay");
    globalSustain = std::make_unique<ParameterInstance>(audioProcessor, *this, "globalSustain");
    globalRelease = std::make_unique<ParameterInstance>(audioProcessor, *this, "globalRelease");
    globalModIndex = std::make_unique<ParameterInstance>(audioProcessor, *this, "globalModIndex");
    outputRouting = std::make_unique<ParameterInstance>(audioProcessor, *this, "outputRouting");

}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "port", 1 }, "Glide", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalModIndex", 1 }, "Global Mod Index", juce::NormalisableRange<float>(25.0f, 400.0f, 0.01f), 100.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalAttack", 1 }, "Global Attack", juce::NormalisableRange<float>(25.0f, 400.0f, 0.01f), 100.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalDecay", 1 }, "Global Decay", juce::NormalisableRange<float>(25.0f, 400.0f, 0.01f), 100.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalSustain", 1 }, "Global Sustain", juce::NormalisableRange<float>(25.0f, 400.0f, 0.01f), 100.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalRelease", 1 }, "Global Release", juce::NormalisableRange<float>(25.0f, 400.0f, 0.1f), 100.0f));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "outputRouting", 1 }, "Output Routing", 0, 15, 1));

    std::array<float, 4> defaultAmplitude = { 100.0f, 0.0f, 0.0f, 0.0f };
    std::array<int, 4> defaultRouting = { 2, 4, 8, 0 };

    
    for (int oper = 0; oper < 4; oper++)
    {
        //******** Envelope Controls ********//
        juce::String attackID = "attack" + juce::String(oper);
        juce::String attackName = "Attack " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { attackID, 1 }, attackName, juce::NormalisableRange<float>(0.0f, 20.0f, 0.01f, 0.5f), .01f));
        
        juce::String decayID = "decay" + juce::String(oper);
        juce::String decayName = "Decay " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { decayID, 1 }, decayName, juce::NormalisableRange<float>(0.0f, 20.0f, 0.01f, 0.5f), .2f));

        juce::String sustainID = "sustain" + juce::String(oper);
        juce::String sustainName = "Sustain " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { sustainID, 1 }, sustainName, juce::NormalisableRange<float>(0.0f, 100.0f), 80.0f));

        juce::String releaseID = "release" + juce::String(oper);
        juce::String releaseName = "Release " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { releaseID, 1 }, releaseName, juce::NormalisableRange<float>(0.0f, 20.0f, 0.01f, 0.5f), 1.0f));
        
        //******** Ratio and FM Amount ********//
        juce::String ratioID = "ratio" + juce::String(oper);
        juce::String ratioName = "Ratio " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { ratioID, 1 }, ratioName, juce::NormalisableRange<float>(0.25f, 20.0f, 0.25f), 1.0f));

        juce::String fixedID = "fixed" + juce::String(oper);
        juce::String fixedName = "Fixed " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { fixedID, 1 }, fixedName, juce::NormalisableRange<float>(20.0f, 20000.0f), 20.0f));
        
        juce::String opModeID = "opMode" + juce::String(oper);
        juce::String opModeName = "Mode " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { opModeID, 1},
                                                              opModeName,
                                                               false));

        juce::String amplitudeID = "amplitude" + juce::String(oper);
        juce::String amplitudeName = "Amplitude " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { amplitudeID, 1 }, amplitudeName, juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 0.5f), defaultAmplitude[oper]));
        
        juce::String phaseID = "phase" + juce::String(oper);
        juce::String phaseName = "Phase " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { phaseID, 1 }, phaseName, juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f, 0.5f), 0.0f));
        
        //******** Operator Input ********//
        juce::String operatorRoutingID = "operator" + juce::String(oper) + "Routing";
        juce::String operatorRoutingName = "Op " + juce::String(oper) + " Routing";
        
        layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { operatorRoutingID, 1 }, operatorRoutingName, 0, 15, defaultRouting[oper] ));
    }
    

    return layout;
}

ParameterInstance::ParameterInstance(FledgeAudioProcessor& p, Parameters& pm, juce::String paramID) : audioProcessor(p), param(pm)
{
    this->paramID = paramID;
    
    float initValue = param.apvts.getRawParameterValue(paramID)->load();
    DBG(paramID << ": " << initValue);
    cachedValue.store(initValue);

    if (auto* parameter = dynamic_cast<juce::AudioProcessorParameterWithID*>(param.apvts.getParameter(paramID)))
    {
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(parameter))
        {
            rangedParam = ranged;
            rangedParam->addListener(this);
        }
    }
}

void ParameterInstance::parameterValueChanged (int /*maybe unused*/, float newValue)
{
    // load atomics for thread safe reading
    cachedValue.store(newValue);
    triggerUpdate();
    triggerAsyncUpdate();
}

void ParameterInstance::handleAsyncUpdate()
{
    if (rangedParam)
    {
        float newValue = cachedValue.load(std::memory_order_relaxed);
        
        if (auto* parameter = dynamic_cast<juce::AudioProcessorParameterWithID*>(param.apvts.getParameter(paramID)))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(parameter))
            {
                valueSafe = rangedParam->convertFrom0to1(newValue);
            }
        }
    }
}

void ParameterInstance::triggerUpdate()
{
    if (rangedParam)
    {
        float newValue = cachedValue.load(std::memory_order_relaxed);
        if (auto* parameter = dynamic_cast<juce::AudioProcessorParameterWithID*>(param.apvts.getParameter(paramID)))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(parameter))
            {
                value.store(rangedParam->convertFrom0to1(newValue));
            }
        }
    }
}

float ParameterInstance::get() const noexcept
{
    return value.load(std::memory_order_relaxed);
}

float ParameterInstance::getSafe() const noexcept
{
    return valueSafe;
}
