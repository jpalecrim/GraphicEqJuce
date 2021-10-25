/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GraphicEqAudioProcessor::GraphicEqAudioProcessor()
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

GraphicEqAudioProcessor::~GraphicEqAudioProcessor()
{
}

//==============================================================================
const juce::String GraphicEqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GraphicEqAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GraphicEqAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GraphicEqAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GraphicEqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GraphicEqAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GraphicEqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GraphicEqAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GraphicEqAudioProcessor::getProgramName (int index)
{
    return {};
}

void GraphicEqAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GraphicEqAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto chainSettings = getChainSettings(apvts);

    //-----------------------------------Peaks----------------------------//

    auto peak31 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        31,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak31GainInDecibels));

    leftChain.get<ChainPositions::peak31>().coefficients = *peak31;
    rightChain.get<ChainPositions::peak31>().coefficients = *peak31;

    auto peak62 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        62,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak62GainInDecibels));

    leftChain.get<ChainPositions::peak62>().coefficients = *peak62;
    rightChain.get<ChainPositions::peak62>().coefficients = *peak62;

    auto peak125 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        125,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak125GainInDecibels));

    leftChain.get<ChainPositions::peak125>().coefficients = *peak125;
    rightChain.get<ChainPositions::peak125>().coefficients = *peak125;

    auto peak250 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        250,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak250GainInDecibels));

    leftChain.get<ChainPositions::peak250>().coefficients = *peak250;
    rightChain.get<ChainPositions::peak250>().coefficients = *peak250;

    auto peak500 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        500,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak500GainInDecibels));

    leftChain.get<ChainPositions::peak500>().coefficients = *peak500;
    rightChain.get<ChainPositions::peak500>().coefficients = *peak500;

    auto peak1k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        1000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak1kGainInDecibels));

    leftChain.get<ChainPositions::peak1k>().coefficients = *peak1k;
    rightChain.get<ChainPositions::peak1k>().coefficients = *peak1k;

    auto peak2k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        2000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak2kGainInDecibels));

    leftChain.get<ChainPositions::peak2k>().coefficients = *peak2k;
    rightChain.get<ChainPositions::peak2k>().coefficients = *peak2k;

    auto peak4k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        4000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak4kGainInDecibels));

    leftChain.get<ChainPositions::peak4k>().coefficients = *peak4k;
    rightChain.get<ChainPositions::peak4k>().coefficients = *peak4k;

    auto peak8k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        8000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak8kGainInDecibels));

    leftChain.get<ChainPositions::peak8k>().coefficients = *peak8k;
    rightChain.get<ChainPositions::peak8k>().coefficients = *peak8k;

    auto peak16k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        16000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak16kGainInDecibels));

    leftChain.get<ChainPositions::peak16k>().coefficients = *peak16k;
    rightChain.get<ChainPositions::peak16k>().coefficients = *peak16k;
    
  //-------------------------------------------Filters--------------------------------------------------------//

    auto Lowcut = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, chainSettings.lowCutFreq, 1);
    leftChain.get<ChainPositions::Lowcut>().coefficients = *Lowcut;
    rightChain.get<ChainPositions::Lowcut>().coefficients = *Lowcut;

    auto HiCut = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, chainSettings.hiCutFreq, 1);
    leftChain.get<ChainPositions::HiCut>().coefficients = *HiCut;
    rightChain.get<ChainPositions::HiCut>().coefficients = *HiCut;
    
}

void GraphicEqAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GraphicEqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void GraphicEqAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
   
    //-------------------processamento dos peaks---------------------------------------//

    auto chainSettings = getChainSettings(apvts);
    auto peak31 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        31,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak31GainInDecibels));

    leftChain.get<ChainPositions::peak31>().coefficients = *peak31;
    rightChain.get<ChainPositions::peak31>().coefficients = *peak31;

    auto peak62 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        62,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak62GainInDecibels));

    leftChain.get<ChainPositions::peak62>().coefficients = *peak62;
    rightChain.get<ChainPositions::peak62>().coefficients = *peak62;

    auto peak125 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        125,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak125GainInDecibels));

    leftChain.get<ChainPositions::peak125>().coefficients = *peak125;
    rightChain.get<ChainPositions::peak125>().coefficients = *peak125;

    auto peak250 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        250,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak250GainInDecibels));

    leftChain.get<ChainPositions::peak250>().coefficients = *peak250;
    rightChain.get<ChainPositions::peak250>().coefficients = *peak250;

    auto peak500 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        500,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak500GainInDecibels));

    leftChain.get<ChainPositions::peak500>().coefficients = *peak500;
    rightChain.get<ChainPositions::peak500>().coefficients = *peak500;

    auto peak1k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        1000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak1kGainInDecibels));

    leftChain.get<ChainPositions::peak1k>().coefficients = *peak1k;
    rightChain.get<ChainPositions::peak1k>().coefficients = *peak1k;

    auto peak2k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        2000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak2kGainInDecibels));

    leftChain.get<ChainPositions::peak2k>().coefficients = *peak2k;
    rightChain.get<ChainPositions::peak2k>().coefficients = *peak2k;

    auto peak4k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        4000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak4kGainInDecibels));

    leftChain.get<ChainPositions::peak4k>().coefficients = *peak4k;
    rightChain.get<ChainPositions::peak4k>().coefficients = *peak4k;

    auto peak8k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        8000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak8kGainInDecibels));

    leftChain.get<ChainPositions::peak8k>().coefficients = *peak8k;
    rightChain.get<ChainPositions::peak8k>().coefficients = *peak8k;

    auto peak16k = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        16000,
        0.707,
        juce::Decibels::decibelsToGain(chainSettings.peak16kGainInDecibels));

    leftChain.get<ChainPositions::peak16k>().coefficients = *peak16k;
    rightChain.get<ChainPositions::peak16k>().coefficients = *peak16k;
    
    //--------------------prcoessamento dos filtros-------------------------------------//

    auto Lowcut = juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), chainSettings.lowCutFreq, 1);

    leftChain.get<ChainPositions::Lowcut>().coefficients = *Lowcut;
    rightChain.get<ChainPositions::Lowcut>().coefficients = *Lowcut;

    auto HiCut = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), chainSettings.hiCutFreq, 1);
    leftChain.get<ChainPositions::HiCut>().coefficients = *HiCut;
    rightChain.get<ChainPositions::HiCut>().coefficients = *HiCut;


    //----------------------------------------- processamento do plugin--------------//  
    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    leftChain.process(leftContext);
    rightChain.process(rightContext);

}

//==============================================================================
bool GraphicEqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GraphicEqAudioProcessor::createEditor()
{
    //return new GraphicEqAudioProcessorEditor (*this); 
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void GraphicEqAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GraphicEqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    settings.lowCutFreq = apvts.getRawParameterValue("LowCut")->load();
    settings.hiCutFreq = apvts.getRawParameterValue("HiCut")->load();

    settings.peak31GainInDecibels = apvts.getRawParameterValue("peak31")->load();
    settings.peak62GainInDecibels = apvts.getRawParameterValue("peak62")->load();
    settings.peak125GainInDecibels = apvts.getRawParameterValue("peak125")->load();
    settings.peak250GainInDecibels = apvts.getRawParameterValue("peak250")->load();
    settings.peak500GainInDecibels = apvts.getRawParameterValue("peak500")->load();
    settings.peak1kGainInDecibels = apvts.getRawParameterValue("peak1k")->load();
    settings.peak2kGainInDecibels = apvts.getRawParameterValue("peak2k")->load();
    settings.peak4kGainInDecibels = apvts.getRawParameterValue("peak4k")->load();
    settings.peak8kGainInDecibels = apvts.getRawParameterValue("peak8k")->load();
    settings.peak16kGainInDecibels = apvts.getRawParameterValue("peak16k")->load();


    return settings;
}
juce::AudioProcessorValueTreeState::ParameterLayout
    GraphicEqAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut",
                                                           "High Pass",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                           20.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HiCut",
                                                           "Low Pass",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                           20000.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("peak31",
                                                           "31 Hz",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("peak62",
                                                           "62 Hz",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("peak125",
                                                           "125 Hz",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("peak250",
                                                           "250 Hz",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("peak500",
                                                           "500 Hz",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("peak1k",
                                                           "1k Hz",
                                                            juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                            0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("peak2k",
                                                           "2k Hz",
                                                            juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                            0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("peak4k",
                                                           "4k Hz",
                                                            juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                            0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("peak8k",
                                                           "8k Hz",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("peak16k",
                                                           "16k Hz",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));

    return layout;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GraphicEqAudioProcessor();
}
