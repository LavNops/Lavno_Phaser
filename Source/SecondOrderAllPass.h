/*
  ==============================================================================

    SecondOrderAllPass.h
    Created: 18 Sep 2022 6:25:42pm
    Author:  Lavno

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#define _USE_MATH_DEFINES
#include <math.h>

struct AllPassFilterParameters
{
    float mfc = 100;
    float mQ = 0.707;
    
};

class SecondOrderAllPass
{
public:
    SecondOrderAllPass();
    ~SecondOrderAllPass();
    void prepare(const juce::dsp::ProcessSpec spec, const AllPassFilterParameters& parameters);
    void processBlock(const juce::dsp::ProcessContextReplacing<float>& context);
    float processSample(float& sample, int& channel);
    void setParameters(const AllPassFilterParameters& parameters);
    AllPassFilterParameters getParameters();
    void updateCoefficients();
    float getDelayedComponent(const int& channel);
    float getG_Value(const int& channel);

private:

    float mSampleRate = 44100;
    int mNumOfChannel = 2;
    float _a1{ 0.f }, _a2{ 0.f };
    
    std::array<float, 2>xPre1{ 0.f };
    std::array<float, 2>xPre2{ 0.f };
    std::array<float, 2>yPre1{ 0.f };
    std::array<float, 2>yPre2{ 0.f };

    std::array<float, 2>lastOnes{ 0.f,0.f };
    std::array<float, 2>lastTwos{ 0.f,0.f };
    AllPassFilterParameters params;


};