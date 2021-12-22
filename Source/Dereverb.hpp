//
//  AdaptiveFilter.hpp
//  De-Reverb - AU
//
//  Created by Ryan Miller on 2/8/20.

/*
 This class implements a blind dereverberation effect inspired by the following paper:

 "Adaptive Dereverberation method based on complementary wiener filter and modulation transfer function"
 K. Ohtani, T. Komatsu, T. Nishino, K. Takeda, REVERB Workshop 2014

 Modifications include removal of spectral subtraction block and
 user-controlled instead of adaptive alpha values.
 
 Issues in implementing the adaptive algorithm, in particular the frequency response of
 the maskingGain as shown in Equation (16) in the paper, rendered the adaptive functionality
 non-working, so user controlled alpha's were implemented to control amount of de-reverberation
 */

#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <JuceHeader.h>

using namespace std;

class Dereverb
{
public:
    void setAlpha(float dereverbPercent); // dereverbPercent == [0.f,100.f]
    void processBuffer(HeapBlock<dsp::Complex<float>> &frequencyDomainBuffer, int numSamples);
#if UNUSED
    void processBlock(float *fftChannelData, int numSamples);
#endif

private:
    // Alpha values used to control amount of reverb reduction
    float alpha1 = 0.8f;
    float alpha2 = 0.2f;
        
    // R1, R2 are the averaged power spectrum values. Calculated using exponential moving average
    float R1[4096] = {0.0f};
    float R2[4096] = {0.0f};
    float R1Past[4096] = {0.0f};
    float R2Past[4096] = {0.0f};
    
    float gainFromUpdatingR1R2(float inputPower, int bin);
    float calculateMaskingGain(float R1, float R2) const;
};
