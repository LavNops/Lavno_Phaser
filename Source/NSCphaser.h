/*
  ==============================================================================

    NSCphaser.h
    Created: 19 Sep 2022 8:05:29pm
    Author:  hp

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SecondOrderAllPass.h"
#include "LFO.h"


struct PhaserParameters 
{
    float phs_Rate = 2.f;
    float phs_Depth = 20.f;
    float phs_Feed = 50.0f;
    float phs_Stereo = 0.f;
    float phs_mix = 100.f;
    LFOParameters lfoParams;
};



class NSCPhaser
{
public:
    NSCPhaser();
    ~NSCPhaser();
    void prepare(const juce::dsp::ProcessSpec& spec);
    void processBuffer(juce::AudioBuffer<float>& buffer);
    void setParameters(const int channel);
    void setLFOParameters(const int channel);
    void setAllPassFilterParameters(const int channel);
    void setPhaserParameters(const PhaserParameters& updatedParams);
    float returnModulatedFc(double modulatorValue, double minFc, double maxFc);

    PhaserParameters phaserParams;
private:

    std::array<SecondOrderAllPass, 6> allPassFilterBank;
    const int numFilter = 6;

    double mSampleRate = 44100;
    double mNumChannels = 2;
    double bufferSize = 512;
    double notchWidth = 1.f;
    

    

    std::array<double, 6> minFc{ 16.0,33.0,48.0,98.0,160.0,260.0 };
    std::array<double, 6> maxFc{ 1600.0,3300.0,4800.0,9800.0,16000.0,20480.0 };
    std::array<double, 6> mQ{ 0.707 };
    std::array<float, 6> mGamma{ 0.f };

    std::array<LFO, 2> mLFOs;
    

    float modulatorValue = 0.f;
    

};