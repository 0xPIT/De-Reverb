/*
*/

#pragma once

#include <JuceHeader.h>
#include "Dereverb.hpp"
#include "STFT.h"
#include "VUAnalysis.h"

class DereverbAudioProcessor  : public AudioProcessor
{
public:
    DereverbAudioProcessor();
    ~DereverbAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
public:
    // Holds the states of all parameters
    AudioProcessorValueTreeState state;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // FFT Object from GitHub Freq Domain Template (https://github.com/juandagilc/Audio-Effects/tree/master/Template%20Frequency%20Domain)
    STFT stft;
    
    // Meter value parameter, connects to front end.
    // std::atomic<> makes sure it doesn't interrupt audio processing thread
    std::atomic<float> inputMeterValue;
    std::atomic<float> outputMeterValue;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DereverbAudioProcessor)
        
    const int fftSize = 4096;
    const int fftHopSize = 8; // hop size factor; hop samples = fftSize / hopSize
        
    VUAnalysis inputvuAnalysis;
    VUAnalysis outputvuAnalysis;
    float inValue[2] = { 0.0f };
    float outValue[2] = { 0.0f };
    
    //==============================================================================
};
