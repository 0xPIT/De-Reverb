//
//  AdaptiveFilter.cpp
//  De-Reverb - AU
//
//  Created by Ryan Miller on 2/8/20.
//

#include <cstdint>
#include <type_traits>
#include "Dereverb.hpp"

using namespace std;

void Dereverb::setAlpha(float dereverbPercent)
{
    // Update alpha1 and alpha2
    // NOTE:
    //  o alpha1 + alpha2 ~= 1.0
    //  o alpha1 = [0.0, 0.49]
    //  o alpha2 = [0.5, 0.99]
 
    float normPercent = dereverbPercent / 100.f; // normalize the de-reverb percentage to scale of 0-1
    alpha2 = 0.5f + normPercent * 0.49f;
    alpha1 = 1.f - alpha2;
}

float Dereverb::calculateMaskingGain(float R1, float R2) const
{
    float maskingGain = 0.0f;

    if ((R1 / R2) >= 1) {
        maskingGain = 1.f;
    }
    else {
        // Check if R2 is <= 0 to avoid division by 0 or negative values
        float epsilon = numeric_limits<double>::epsilon();
        if (R2 <= epsilon) {
            maskingGain = 0.f;
        }
        else {
            maskingGain = R1 / R2;
        }
    }
    
    return maskingGain;
}

float Dereverb::gainFromUpdatingR1R2(float inputPower, int bin)
{
    // Paper: RX = (1 − α) * PX + α * RXpast
    // has input (PX) and average (RX) swapped compared to most moving exp. avg. implementations

    R1[bin] = (1.0f - alpha1) * inputPower + (alpha1 * R1Past[bin]);
    R2[bin] = (1.0f - alpha2) * inputPower + (alpha2 * R2Past[bin]);

    R1Past[bin] = R1[bin];
    R2Past[bin] = R2[bin];
    
    // Set new masking gain after updating R1, R2
    return calculateMaskingGain(R1[bin], R2[bin]);
}

void Dereverb::processBuffer(HeapBlock<dsp::Complex<float>> &frequencyDomainBuffer, int numSamples)
{
    for (int i = 0; i < numSamples; i++) {
        float real = frequencyDomainBuffer[i].real();
        float imag = frequencyDomainBuffer[i].imag();
        
        float P = powf(abs(frequencyDomainBuffer[i]), 2);
        float maskingGain = gainFromUpdatingR1R2(P, i);
        
        // Apply masking gain to real and imaginary parts
        frequencyDomainBuffer[i].real(real * maskingGain);
        frequencyDomainBuffer[i].imag(imag * maskingGain);
    }
}

#if UNUSED
void Dereverb::processBlock(float* fftChannelData, int numSamples)
{
    for (int i = 0; i < numSamples; i += 2) {
        float P = sqrtf(pow(fftChannelData[i], 2) + pow(fftChannelData[i + 1], 2));
        float maskingGain = gainFromUpdatingR1R2(P, i);
        
        fftChannelData[i] *= maskingGain;
        fftChannelData[i + 1] *= maskingGain;
    }
}
#endif
