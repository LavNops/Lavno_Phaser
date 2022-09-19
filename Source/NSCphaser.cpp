/*
  ==============================================================================

    NSCphaser.cpp
    Created: 19 Sep 2022 8:05:29pm
    Author:  hp

  ==============================================================================
*/

#include "NSCphaser.h"


NSCPhaser::NSCPhaser()
{

}


NSCPhaser::~NSCPhaser()
{

}


void NSCPhaser::prepare(const juce::dsp::ProcessSpec& spec)
{
    mSampleRate = spec.sampleRate;
    mNumChannels = spec.numChannels;
    bufferSize = spec.maximumBlockSize;

    for (int i = 0; i < numFilter; ++i)
    {
        auto parameters = allPassFilterBank[i].getParameters();
        auto filterFc = minFc[i] + 1000;
        parameters.mfc = filterFc;
        mQ[i] = filterFc / notchWidth;
        parameters.mQ = mQ[i];
        allPassFilterBank[i].prepare(spec, parameters);
    }
}


void NSCPhaser::processBuffer(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < mNumChannels; ++channel)
    {
        auto inputBlock = buffer.getReadPointer(channel);
        auto outputBlock = buffer.getWritePointer(channel);
        bufferSize = buffer.getNumSamples();

        for (int sample = 0; sample < bufferSize; ++sample)
        {

            for (int i = 0; i < numFilter; ++i)
            {
                auto parameters = allPassFilterBank[i].getParameters();
                auto filterFc = minFc[i];
                parameters.mfc = filterFc + 1000;
                mQ[i] = filterFc / notchWidth;
                parameters.mQ = mQ[i];

                allPassFilterBank[i].setParameters(parameters);

            }

            mGamma[5] = allPassFilterBank[5].getG_Value(channel);
            mGamma[4] = allPassFilterBank[4].getG_Value(channel) * mGamma[5];
            mGamma[3] = allPassFilterBank[3].getG_Value(channel) * mGamma[4];
            mGamma[2] = allPassFilterBank[2].getG_Value(channel) * mGamma[3];
            mGamma[1] = allPassFilterBank[1].getG_Value(channel) * mGamma[2];
            mGamma[0] = allPassFilterBank[0].getG_Value(channel) * mGamma[1];

            float mK = mIntensity / 100.0;
            float alpha0 = 1.0 / (1.0 + mK * mGamma[0]);
            float Sn = mGamma[1] * allPassFilterBank[0].getDelayedComponent(channel)+ 
                        mGamma[2] * allPassFilterBank[1].getDelayedComponent(channel)+ 
                        mGamma[3] * allPassFilterBank[2].getDelayedComponent(channel)+
                        mGamma[4] * allPassFilterBank[3].getDelayedComponent(channel)+
                        mGamma[5] * allPassFilterBank[4].getDelayedComponent(channel)+
                        allPassFilterBank[5].getDelayedComponent(channel);


            float inputSample = alpha0 * (inputBlock[sample] - mK * Sn);
            /*float inputSample = inputBlock[sample];*/
            float processedSample = 0.f;

            for (int i = 0; i < numFilter; ++i)
            {
                processedSample = allPassFilterBank[i].processSample(inputSample, channel);
                inputSample = processedSample;
            }

            outputBlock[sample] = 0.707 * inputBlock[sample] + 0.707 * processedSample;
        }
    }
}
    

