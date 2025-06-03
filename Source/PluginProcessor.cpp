/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "VoiceProcessor.h"

//==============================================================================
FledgeAudioProcessor::FledgeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

FledgeAudioProcessor::~FledgeAudioProcessor()
{
}

//==============================================================================
const juce::String FledgeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FledgeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FledgeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FledgeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FledgeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FledgeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FledgeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FledgeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FledgeAudioProcessor::getProgramName (int index)
{
    return {};
}

void FledgeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FledgeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.addSound(new SynthSound());
    synth.addVoice(new SynthVoice());
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int v = 0; v < synth.getNumVoices(); v++)
    {
        if(auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(v)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

void FledgeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FledgeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FledgeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
//    auto totalNumInputChannels  = getTotalNumInputChannels();
  //  auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int oper = 0; oper < 4; oper++){
        juce::String attackID = "attack" + juce::String(oper);
        juce::String decayID = "decay" + juce::String(oper);
        juce::String sustainID = "sustain" + juce::String(oper);
        juce::String releaseID = "release" + juce::String(oper);

        float attack = apvts.getRawParameterValue(attackID)->load();
        float decay = apvts.getRawParameterValue(decayID)->load();
        float sustain = apvts.getRawParameterValue(sustainID)->load();
        float release = apvts.getRawParameterValue(releaseID)->load();

        juce::String ratioID = "ratio" + juce::String(oper);
        juce::String fixedID = "fixed" + juce::String(oper);
        juce::String modIndexID = "modIndex" + juce::String(oper);

        float ratio = apvts.getRawParameterValue(ratioID)->load();
        float fixed = apvts.getRawParameterValue(fixedID)->load();
        float modIndex = apvts.getRawParameterValue(modIndexID)->load();

        for (int v = 0; v < synth.getNumVoices(); v++)
        {
            if(auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(v)))
            {
                voice->setEnvelope(oper, attack, decay, sustain/100.0f, release);
                voice->setFMParameters(oper, ratio, fixed, false, modIndex);
            }
        }
    }
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool FledgeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FledgeAudioProcessor::createEditor()
{
    return new FledgeAudioProcessorEditor (*this);
}

//==============================================================================
void FledgeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FledgeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FledgeAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout FledgeAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    for (int oper = 0; oper < 4; oper++)
    {
        //******** Envelope Controls ********//
        juce::String attackID = "attack" + juce::String(oper);
        juce::String attackName = "Attack " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { attackID, 1 }, attackName, juce::NormalisableRange<float>(0.0f, 20.0f, 0.1f, 0.5f), 1.0f));
        
        juce::String decayID = "decay" + juce::String(oper);
        juce::String decayName = "Decay " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { decayID, 1 }, decayName, juce::NormalisableRange<float>(0.0f, 20.0f, 0.1f, 0.5f), 1.0f));

        juce::String sustainID = "sustain" + juce::String(oper);
        juce::String sustainName = "Sustain " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { sustainID, 1 }, sustainName, juce::NormalisableRange<float>(0.0f, 100.0f), 80.0f));

        juce::String releaseID = "release" + juce::String(oper);
        juce::String releaseName = "Release " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { releaseID, 1 }, releaseName, juce::NormalisableRange<float>(0.0f, 20.0f, 0.1f, 0.5f), 1.0f));
        
        //******** Ratio and FM Amount ********//
        juce::String ratioID = "ratio" + juce::String(oper);
        juce::String ratioName = "Ratio " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { ratioID, 1 }, ratioName, juce::NormalisableRange<float>(0.25f, 20.0f, 0.25f), 1.0f));

        juce::String fixedID = "fixed" + juce::String(oper);
        juce::String fixedName = "Fixed " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { fixedID, 1 }, fixedName, juce::NormalisableRange<float>(20.0f, 20000.0f), 20.0f));
        
        juce::String opModeID = "opMode" + juce::String(oper);
        juce::String opModeName = "Mode " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { opModeID, 1 }, opModeName, juce::NormalisableRange<float>(20.0f, 20000.0f), 20.0f));

        juce::String modIndexID = "modIndex" + juce::String(oper);
        juce::String modIndexName = "Modulation Amount " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { modIndexID, 1 }, modIndexName, juce::NormalisableRange<float>(0.0f, 10.0f), 0.0f));
    }

    return layout;
}
