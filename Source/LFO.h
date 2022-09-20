/*
  ==============================================================================

    LFO.h
    Created: 19 Sep 2022 9:58:31pm
    Author:  hp

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define _USE_MATH_DEFINES
#include <math.h>

enum WaveformType
{
    mSine,
    mSaw,
    mTriangle
};

struct LFOParameters
{
    double oscFrequency = 100.0;
    WaveformType mWaveform = WaveformType::mSaw;
};

class LFO
{
public:

    LFO();
    ~LFO();
    void prepare(const juce::dsp::ProcessSpec& spec);
    float getValue();
    float unipolar2Bipolar(const double& modCounter);
    float parabolicSine(float x);
    void setParameters(const LFOParameters& parameters);
    LFOParameters getParameters();

private:

    LFOParameters params;
    double modCounter = 0.0;


    //Quick sine calculation parameters
    const float mB = 4.0 / M_PI;
    const float mC = -4.0 / (M_PI * M_PI);
    const float mP = 0.225;

    double mSampleRate = 44100;
    double phaseInc = 0.1;
};
