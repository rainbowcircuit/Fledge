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
    const auto params = this->getParameters();
    for (auto param : params){
        param->addListener(this);
    }
}

FledgeAudioProcessor::~FledgeAudioProcessor()
{
    const auto params = this->getParameters();
    for (auto param : params){
        param->removeListener(this);
    }
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
        
    
    float globalAttack = apvts.getRawParameterValue("globalAttack")->load();
    float globalDecay = apvts.getRawParameterValue("globalDecay")->load();
    float globalSustain = apvts.getRawParameterValue("globalSustain")->load();
    float globalRelease = apvts.getRawParameterValue("globalRelease")->load();
    
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
        juce::String modIndexID = "amplitude" + juce::String(oper);
        juce::String operatorRoutingID = "operator" + juce::String(oper) + "Routing";

        float ratio = apvts.getRawParameterValue(ratioID)->load();
        float fixed = apvts.getRawParameterValue(fixedID)->load();
        float modIndex = apvts.getRawParameterValue(modIndexID)->load();
        float routing = apvts.getRawParameterValue(operatorRoutingID)->load();

        for (int v = 0; v < synth.getNumVoices(); v++)
        {
            if(auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(v)))
            {
                voice->setEnvelope(oper, attack, decay, sustain/100.0f, release,
                                   globalAttack, globalDecay, globalSustain, globalRelease);
                voice->setFMParameters(oper, ratio, fixed, false, modIndex);
                voice->setOperatorGain(oper, routing);
                levelAtomic.store(voice->getOutputSample());
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
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void FledgeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
       if (xmlState == nullptr)
           return;
       const auto newTree = juce::ValueTree::fromXml(*xmlState);
       apvts.replaceState(newTree);
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
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "port", 1 }, "Glide", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalAttack", 1 }, "Global Attack", juce::NormalisableRange<float>(-100.0f, 100.0f, 0.01f), 0.01f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalDecay", 1 }, "Global Decay", juce::NormalisableRange<float>(-100.0f, 100.0f, 0.01f), 0.2f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalSustain", 1 }, "Global Sustain", juce::NormalisableRange<float>(-100.0f, 100.0f, 0.01f), 80.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalRelease", 1 }, "Global Release", juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f), 1.0f));

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
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { opModeID, 1 }, opModeName, juce::NormalisableRange<float>(20.0f, 20000.0f), 20.0f));

        juce::String modIndexID = "amplitude" + juce::String(oper);
        juce::String modIndexName = "Modulation Amount " + juce::String(oper);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { modIndexID, 1 }, modIndexName, juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 0.5f), 0.0f));
        
        //******** Operator Input ********//
        juce::String operatorRoutingID = "operator" + juce::String(oper) + "Routing";
        juce::String operatorRoutingName = "Operator " + juce::String(oper) + " Routing";
        
        layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { operatorRoutingID, 1 }, operatorRoutingName, 0, 15, 0));
        
    }
    
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "outputRouting", 1 }, "Output Routing", 0, 15, 0));

    return layout;
}
