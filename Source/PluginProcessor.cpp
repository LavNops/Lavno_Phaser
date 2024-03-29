/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Lavno_PhaserAudioProcessor::Lavno_PhaserAudioProcessor()
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
    apvts.addParameterListener("Rate", this);
    apvts.addParameterListener("Depth", this);
    apvts.addParameterListener("Feed", this);
    apvts.addParameterListener("Mix", this);
    apvts.addParameterListener("LFOType", this);
    

}

Lavno_PhaserAudioProcessor::~Lavno_PhaserAudioProcessor()
{
    apvts.removeParameterListener("Rate", this);
    apvts.removeParameterListener("Depth", this);
    apvts.removeParameterListener("Feed", this);
    apvts.removeParameterListener("Mix", this);
    apvts.removeParameterListener("LFOType", this);
}

//==============================================================================
const juce::String Lavno_PhaserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Lavno_PhaserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Lavno_PhaserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Lavno_PhaserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Lavno_PhaserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Lavno_PhaserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Lavno_PhaserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Lavno_PhaserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Lavno_PhaserAudioProcessor::getProgramName (int index)
{
    return {};
}

void Lavno_PhaserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Lavno_PhaserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getNumInputChannels();
    phaser.prepare(spec);
}

void Lavno_PhaserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Lavno_PhaserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Lavno_PhaserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    phaser.processBuffer(buffer);
    
    
}

//==============================================================================
bool Lavno_PhaserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Lavno_PhaserAudioProcessor::createEditor()
{
    //return new Lavno_PhaserAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Lavno_PhaserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Lavno_PhaserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Lavno_PhaserAudioProcessor();
    
}


juce::AudioProcessorValueTreeState::ParameterLayout Lavno_PhaserAudioProcessor::createParameterLayout()
{

    juce::StringArray LFOType = { "Sine","Saw","Triangle" };
    juce::String LFOLabel = { "LFO Type" };
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("Rate", "Rate", juce::NormalisableRange<float>(0.f, 20.f, 0.01f), 2.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Depth", "Depth", juce::NormalisableRange<float>(0.f, 100.f, 1.f), 20.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Feed", "Feed", juce::NormalisableRange<float>(0.f, 100.f, 1.f), 50.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mix", "Mix", juce::NormalisableRange<float>(0.f, 100.f, 1.f), 100.f));

    layout.add(std::make_unique<juce::AudioParameterChoice>("LFOType", 
                                                            "LFOType", 
                                                            LFOType, 0, 
                                                            LFOLabel, nullptr, nullptr));

    return layout;
}
   

PhaserParameters getRawValueFromApvts(const juce::AudioProcessorValueTreeState& apvts)
{
    PhaserParameters params;
    params.phs_Rate = apvts.getRawParameterValue("Rate")->load();
    params.phs_Depth = apvts.getRawParameterValue("Depth")->load();
    params.phs_Feed = apvts.getRawParameterValue("Feed")->load();
    params.phs_mix = apvts.getRawParameterValue("Mix")->load();
    params.lfoParams.mWaveform = static_cast<WaveformType>(apvts.getRawParameterValue("LFOType")->load());
    return params;
}


void Lavno_PhaserAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    auto updatedParams=getRawValueFromApvts(apvts);
    phaser.setPhaserParameters(updatedParams);
   
}