/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Waveshaper01AudioProcessor::Waveshaper01AudioProcessor()
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
    state = new AudioProcessorValueTreeState(*this, nullptr);
    state->createAndAddParameter("drive", "Drive", "Drive", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0, nullptr, nullptr);
    state->createAndAddParameter("range", "Range", "Range", NormalisableRange<float>(0.0f, 6666.6f, 0.01f), 0, nullptr, nullptr);
    state->createAndAddParameter("blend", "Blend", "Blend", NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0, nullptr, nullptr);
    state->createAndAddParameter("volume", "Volume", "Volume", NormalisableRange<float>(0.0f, 3.0f, 0.01f), 0, nullptr, nullptr);

    state->state = ValueTree("drive");
    state->state = ValueTree("range");
    state->state = ValueTree("blend");
    state->state = ValueTree("volume");
}

Waveshaper01AudioProcessor::~Waveshaper01AudioProcessor()
{
}

//==============================================================================
const juce::String Waveshaper01AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Waveshaper01AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Waveshaper01AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Waveshaper01AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Waveshaper01AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Waveshaper01AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Waveshaper01AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Waveshaper01AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Waveshaper01AudioProcessor::getProgramName (int index)
{
    return {};
}

void Waveshaper01AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Waveshaper01AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Waveshaper01AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Waveshaper01AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void Waveshaper01AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    float drive = *state->getRawParameterValue("drive");
    float range = *state->getRawParameterValue("range");
    float blend = *state->getRawParameterValue("blend");
    float volume = *state->getRawParameterValue("volume");
    
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            float cleanSignal = *channelData;
            
            *channelData *= drive * range;
            
            *channelData = (((((2.0f / float_Pi) * atan(*channelData) * blend)) + (cleanSignal * (1.0f - blend))) / 2) * volume;
            
            channelData++;
        }
        
    }
}

AudioProcessorValueTreeState& Waveshaper01AudioProcessor::getState()
{
    return *state;
}

//==============================================================================
bool Waveshaper01AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Waveshaper01AudioProcessor::createEditor()
{
    return new Waveshaper01AudioProcessorEditor (*this);
}

//==============================================================================
void Waveshaper01AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    MemoryOutputStream stream(destData, false);
    state->state.writeToStream(stream);
}

void Waveshaper01AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    ValueTree tree = ValueTree::readFromData(data, sizeInBytes);
    
    if (tree.isValid())
    {
        state->state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Waveshaper01AudioProcessor();
}
