/*

 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

DereverbAudioProcessor::DereverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
         #if ! JucePlugin_IsMidiEffect
           #if ! JucePlugin_IsSynth
             .withInput  ("Input",  AudioChannelSet::stereo(), true)
           #endif
           .withOutput ("Output", AudioChannelSet::stereo(), true)
         #endif
           ), state(*this, nullptr, Identifier("DereverbParameters"), createParameterLayout())
#endif
{
}

DereverbAudioProcessor::~DereverbAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout DereverbAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
                                                          //Identifier  DAW Display  [  Range  ]  IC
    params.push_back(std::make_unique<AudioParameterFloat>("DEREVERB", "De-Reverb", 0.0f, 100.0f, 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("MAKEUPGAIN", "Make-up Gain", -6.0f, 6.0f, 0.0f));
    params.push_back(std::make_unique<AudioParameterBool>("BYPASS", "Bypass", false));
    
    return { params.begin(), params.end() };
}

void DereverbAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    bool bypassed = *state.getRawParameterValue("BYPASS");
    float gain = 1.0f;

    if (!bypassed) {
        float dBGain = *state.getRawParameterValue("MAKEUPGAIN");
        gain = Decibels::decibelsToGain(dBGain);
        
        float dereverbPercent = *state.getRawParameterValue("DEREVERB");
        stft.dereverbFilter->setAlpha(dereverbPercent);
        stft.processBlock(buffer);
    }
    
    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
        for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
            float inputSample = buffer.getReadPointer(channel)[sample];
            float outputSample = inputSample;

            if (!bypassed) {
                outputSample = inputSample * gain;
            }
            buffer.getWritePointer(channel)[sample] = outputSample;

            inValue[channel] = inputvuAnalysis.processSample(inputSample, channel);
            outValue[channel] = outputvuAnalysis.processSample(outputSample, channel);
        }
    }
    
    // Assign meter value to the max value of L, R channels
    inputMeterValue = jmax(inValue[0], inValue[1]);
    outputMeterValue = jmax(outValue[0], outValue[1]);
}

const String DereverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DereverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DereverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DereverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DereverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DereverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DereverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DereverbAudioProcessor::setCurrentProgram(int index)
{
}

const String DereverbAudioProcessor::getProgramName(int index)
{
    return {};
}

void DereverbAudioProcessor::changeProgramName(int index, const String& newName)
{
}

void DereverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize FFT settings
    stft.setup(getTotalNumInputChannels());
    stft.updateParameters(fftSize, fftHopSize, stft.windowTypeHann);
    
    // Update FS for VU analysis
    inputvuAnalysis.setSampleRate(sampleRate);
    outputvuAnalysis.setSampleRate(sampleRate);
}

void DereverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DereverbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo()) {
        return false;
    }

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) {
        return false;
    }
   #endif

    return true;
  #endif
}
#endif

bool DereverbAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* DereverbAudioProcessor::createEditor()
{
    return new DereverbAudioProcessorEditor (*this);
}

void DereverbAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto currentState = state.copyState();
    std::unique_ptr<XmlElement> xml(currentState.createXml());
    copyXmlToBinary(*xml, destData);
}

void DereverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
        
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState && xmlState->hasTagName(state.state.getType())){
        state.replaceState(ValueTree::fromXml(*xmlState));
    }
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DereverbAudioProcessor();
}
