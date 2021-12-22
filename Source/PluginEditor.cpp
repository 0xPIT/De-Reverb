/*
   
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DereverbAudioProcessorEditor::DereverbAudioProcessorEditor(DereverbAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(400, 240);
    const int sliderY = 80;
    const int sliderWH = 120;

    // REVERB REDUCTION SLIDER
    reverbReductionSlider.setSliderStyle(Slider::Rotary);
    reverbReductionSlider.setRange(0.0f, 100.0f, 1.f); // value in %
    reverbReductionSlider.setBounds(30, sliderY, sliderWH, sliderWH);
    reverbReductionSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(reverbReductionSlider);
    sliderAttachment.emplace_back(
        new AudioProcessorValueTreeState::SliderAttachment(
            processor.state, "DEREVERB", reverbReductionSlider)
    );

    reverbSliderLabel.setText("De-Reverb", dontSendNotification);
    reverbSliderLabel.attachToComponent(&reverbReductionSlider, false);
    reverbSliderLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(reverbSliderLabel);
        
    // MAKEUP GAIN SLIDER
    makeupGainSlider.setSliderStyle(Slider::Rotary);
    makeupGainSlider.setRange(-6.0f, 6.0f, 0.01f); // value in dB
    makeupGainSlider.setBounds(160, sliderY, sliderWH, sliderWH);
    makeupGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(makeupGainSlider);
    sliderAttachment.emplace_back(
        new AudioProcessorValueTreeState::SliderAttachment(processor.state, "MAKEUPGAIN", makeupGainSlider)
    );

    makeupGainLabel.setText("Make-up Gain", dontSendNotification);
    makeupGainLabel.attachToComponent(&makeupGainSlider, false);
    makeupGainLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(makeupGainLabel);
    
    // BYPASS BUTTON
    bypassButton.setBounds(145, 155, 24, 24);
    bypassButton.setClickingTogglesState(true);
    addAndMakeVisible(bypassButton);
    buttonAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(
        processor.state, "BYPASS", bypassButton);
    
    // VU METER
    const int meterW = 10;
    const int meterH = 110;
    const int meterY = 90;
    inputMeter.setBounds(320, meterY, meterW, meterH);
    addAndMakeVisible(inputMeter);
    
    outputMeter.setBounds(350, meterY, meterW, meterH); // x, y, width, height
    addAndMakeVisible(outputMeter);
    
    startTimer(60);
}

DereverbAudioProcessorEditor::~DereverbAudioProcessorEditor() { }

void DereverbAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::whitesmoke);
    g.setFont(30.0f);
    g.drawFittedText("De-Reverb", getLocalBounds(), Justification::centredTop, 1);

    g.setFont(12.0f);
    g.drawFittedText("@0xPIT, Ryan Miller", getLocalBounds(), Justification::bottomRight, 1);
    g.drawFittedText("Bypass", 125, 180, 60, 20, Justification::centred, 1);

    g.setFont(20.0f);
    int sliderTextY = 80+60+10;
    g.drawFittedText("%",   30+60-10, sliderTextY, 20, 20, Justification::centred, 1);
    g.drawFittedText("dB", 160+60-10, sliderTextY, 20, 20, Justification::centred, 1);

    g.setFont(15.0f);
    g.drawFittedText("In",  320-20+5, 60, 40, 20, Justification::centred, 1);
    g.drawFittedText("Out", 350-20+5, 60, 40, 20, Justification::centred, 1);
}

void DereverbAudioProcessorEditor::timerCallback()
{
    inputMeter.update(processor.inputMeterValue);
    outputMeter.update(processor.outputMeterValue);
}

void DereverbAudioProcessorEditor::resized() { }

void DereverbAudioProcessorEditor::sliderValueChanged(Slider *slider) { }

void DereverbAudioProcessorEditor::buttonClicked(Button *button) { }
