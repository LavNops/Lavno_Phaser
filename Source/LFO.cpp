/*
  ==============================================================================

    LFO.cpp
    Created: 19 Sep 2022 9:58:31pm
    Author:  hp

  ==============================================================================
*/

#include "LFO.h"


LFO::LFO()
{

}

LFO::~LFO()
{

}

void LFO::prepare(const juce::dsp::ProcessSpec& spec)
{
    mSampleRate = spec.sampleRate;
    modCounter = 0.0;
    
}


float LFO::getValue()
{
    if (modCounter > 1)
    {
        modCounter -= 1.0;
    }

    float tempOut = 0.f;;
    


    if (WaveformType::mSine)
    {
        float angle = modCounter * 2.0 * M_PI - M_PI;
        tempOut = parabolicSine(-angle);
    }


    if (WaveformType::mSaw)
    {
        tempOut = unipolar2Bipolar(modCounter);
    }


    if (WaveformType::mTriangle)
    {
        tempOut = unipolar2Bipolar(modCounter);
        tempOut = 2.0 * fabs(tempOut) - 1;
    }

    modCounter += phaseInc;

    return tempOut;
}


void  LFO::setParameters(const LFOParameters& parameters)
{
    params.mWaveform = parameters.mWaveform;
    params.oscFrequency = parameters.oscFrequency;
    phaseInc = params.oscFrequency / mSampleRate;
}


LFOParameters LFO::getParameters()
{
    return params;
}

float LFO::unipolar2Bipolar(const double& modCounter)
{
    float y = 2 * modCounter - 1;
    return y;
}


float LFO::parabolicSine(float x)
{
    float y = mB * x + mC * x * fabs(x);
    y = mP * (y * fabs(y) - y)+ y;
    return y;
}