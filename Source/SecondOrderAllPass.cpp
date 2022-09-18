/*
  ==============================================================================

    SecondOrderAllPass.cpp
    Created: 18 Sep 2022 6:25:42pm
    Author:  Lavno

  ==============================================================================
*/

#include "SecondOrderAllPass.h"



SecondOrderAllPass::SecondOrderAllPass()
{

}

SecondOrderAllPass::~SecondOrderAllPass()
{

}

void SecondOrderAllPass::prepare(const juce::dsp::ProcessSpec spec, const AllPassFilterParameters& parameters)
{
    mSampleRate = spec.sampleRate;
    mNumOfChannel = spec.numChannels;
    setParameters(parameters);
}


void SecondOrderAllPass::processBlock(const juce::dsp::ProcessContextReplacing<float>& context)
{
    juce::dsp::AudioBlock<const float> inputBlock= context.getInputBlock();
    juce::dsp::AudioBlock<float> outputBlock = context.getOutputBlock();
    int blockSize = inputBlock.getNumSamples();
    for (int channel = 0; channel < mNumOfChannel; ++channel)
    {
        auto thisChannelInput = inputBlock.getChannelPointer(channel);
        auto thisChannelOutput = outputBlock.getChannelPointer(channel);

        for (int sample = 0; sample < blockSize; ++sample)
        {
            float inputsample = thisChannelInput[sample];
            float tempOut = inputsample + lastOnes[channel] + lastTwos[channel];
            thisChannelOutput[sample] = tempOut;
            xPre2[channel] = xPre1[channel];
            yPre2[channel] = yPre1[channel];
            xPre1[channel] = inputsample;
            yPre1[channel] = tempOut;
            lastOnes[channel] = _a1 * (xPre1[channel] - yPre1[channel]);
            lastTwos[channel] = xPre2[channel] - _a2 * yPre2[channel];
        }
    }
}


void SecondOrderAllPass::setParameters(const AllPassFilterParameters& parameters)
{
    params.mfc = parameters.mfc;
    params.mQ = parameters.mQ < 0 ? 0.707 : parameters.mQ;
    updateCoefficients();

}

AllPassFilterParameters SecondOrderAllPass::getParameters()
{
    return params;
}

void SecondOrderAllPass::updateCoefficients()
{
    float expp = 2.71828182845904523536;
    float mR = pow(expp, -1 * (M_PI * params.mfc) / (params.mQ * mSampleRate));

    _a2 = mR * mR;
    _a1 = -2 * mR * cos(2 * M_PI * params.mfc / mSampleRate);
}