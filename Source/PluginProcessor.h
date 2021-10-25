/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct ChainSettings
{
    float  peak31GainInDecibels{ 0 }, peak62GainInDecibels{ 0 },
           peak125GainInDecibels{ 0 }, peak250GainInDecibels{ 0 },
           peak500GainInDecibels{ 0 },peak1kGainInDecibels{ 0 },
           peak2kGainInDecibels{ 0 }, peak4kGainInDecibels{ 0 },
           peak8kGainInDecibels{ 0 }, peak16kGainInDecibels{ 0 };
          
    float lowCutFreq{ 0 }, hiCutFreq{ 0 };


};
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);
//==============================================================================
/**
*/
class GraphicEqAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GraphicEqAudioProcessor();
    ~GraphicEqAudioProcessor() override;

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
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout()};
  
private:

    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter>;
    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter, Filter,
        Filter, Filter, Filter, Filter, Filter>;

    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        Lowcut,
        peak31,
        peak62,
        peak125,
        peak250,
        peak500,
        peak1k,
        peak2k,
        peak4k,
        peak8k,
        peak16k,
        HiCut
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEqAudioProcessor)
};
