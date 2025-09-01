/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VoiceProcessor.h"
#include "Parameters.h"
#include "Measurement.h"

//==============================================================================
/**
*/
class Parameters;

class FledgeAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FledgeAudioProcessor();
    ~FledgeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void saveEditorState(int width, int height, int index, juce::Point<float> operatorPosition);
    void saveBlockPosition(juce::Point<float> op0Coords, juce::Point<float> op1Coords, juce::Point<float> op2Coords, juce::Point<float> op3Coords);

    
    
    
    Measurement& getOutputLevelL() { return outputLevelL; }
    Measurement& getOutputLevelR() { return outputLevelR; }
    
    
    std::unique_ptr<Parameters> params;

private:
    float globalAttack, globalDecay, globalSustain, globalRelease, globalModIndex, outputRouting;
    bool isFixed;
    std::array<float, 4> attack, decay, sustain, release, ratio, fixed, amplitude, phase, routing;
    
    
    float outputLevel;
    std::atomic<float> levelAtomic;
    
    Measurement outputLevelL;
    Measurement outputLevelR;

    Synthesizer synth;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FledgeAudioProcessor)
};
