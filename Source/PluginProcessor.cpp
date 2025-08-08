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
    params = std::make_unique<Parameters>(*this);
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

    for (int v = 0; v < 8; v++)
        synth.addVoice(new SynthVoice());
    
    synth.addSound(new SynthSound());
    synth.setNoteStealingEnabled(true);
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
        
    for (int oper = 0; oper < 4; oper++){

        for (int v = 0; v < synth.getNumVoices(); v++)
        {
            if(auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(v)))
            {
                voice->setEnvelope(oper,
                                   params->attack[oper]->get(),
                                   params->decay[oper]->get(),
                                   params->sustain[oper]->get(),
                                   params->release[oper]->get(),
                                   params->globalAttack->get(),
                                   params->globalDecay->get(),
                                   params->globalSustain->get(),
                                   params->globalRelease->get());
                
                voice->setFMParameters(oper,
                                       params->ratio[oper]->get(),
                                       fixed[oper],
                                       false,
                                       params->amplitude[oper]->get(),
                                       params->phase[oper]->get(),
                                       params->globalModIndex->get());
                
                voice->setOperatorGain(oper, params->routing[oper]->get(), params->outputRouting->get());
            }
        }
    }
    
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels >= 1) {
        auto* leftData = buffer.getReadPointer(0);
        float maxL = 0.0f;
        for (int i = 0; i < numSamples; ++i) {
            maxL = juce::jmax(maxL, std::abs(leftData[i]));
        }
        outputLevelL.updateIfGreater(maxL);
    }

    if (numChannels >= 2) {
        auto* rightData = buffer.getReadPointer(1);
        float maxR = 0.0f;
        for (int i = 0; i < numSamples; ++i) {
            maxR = juce::jmax(maxR, std::abs(rightData[i]));
        }
        outputLevelR.updateIfGreater(maxR);
    } else if (numChannels >= 1) {
        auto* leftData = buffer.getReadPointer(0);
        float maxL = 0.0f;
        for (int i = 0; i < numSamples; ++i) {
            maxL = juce::jmax(maxL, std::abs(leftData[i]));
        }
        outputLevelR.updateIfGreater(maxL);
    }
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
    float myvalue = 200.0f;
    params->apvts.state.setProperty("testValue", myvalue, nullptr);
    
    copyXmlToBinary(*params->apvts.copyState().createXml(), destData);
}

void FledgeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
       if (xmlState == nullptr)
           return;
       const auto newTree = juce::ValueTree::fromXml(*xmlState);
    params->apvts.replaceState(newTree);
}

void FledgeAudioProcessor::saveEditorState(int width, int height, int index, juce::Point<float> operatorPosition)
{
    params->apvts.state.setProperty("editorWidth", width, nullptr);
    params->apvts.state.setProperty("editorHeight", height, nullptr);
    
    juce::String indexStr = juce::String(index);
    params->apvts.state.setProperty("oper" + indexStr + "XPos", operatorPosition.x, nullptr);
    params->apvts.state.setProperty("oper" + indexStr + "YPos", operatorPosition.y, nullptr);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FledgeAudioProcessor();
}

