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
        allPassFilterBank[i].prepare(spec);
    }

    for (int channel = 0; channel < mNumChannels; ++channel)
    {
        mLFOs[channel].prepare(spec);
        
        setParameters(channel);
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
            setParameters(channel);


            mGamma[5] = allPassFilterBank[5].getG_Value(channel);
            mGamma[4] = allPassFilterBank[4].getG_Value(channel) * mGamma[5];
            mGamma[3] = allPassFilterBank[3].getG_Value(channel) * mGamma[4];
            mGamma[2] = allPassFilterBank[2].getG_Value(channel) * mGamma[3];
            mGamma[1] = allPassFilterBank[1].getG_Value(channel) * mGamma[2];
            mGamma[0] = allPassFilterBank[0].getG_Value(channel) * mGamma[1];

            float mK = phaserParams.phs_Feed / 100.0;
            float alpha0 = 1.0 / (1.0 + mK * mGamma[0]);
            float Sn =  mGamma[1] * allPassFilterBank[0].getDelayedComponent(channel)+ 
                        mGamma[2] * allPassFilterBank[1].getDelayedComponent(channel)+ 
                        mGamma[3] * allPassFilterBank[2].getDelayedComponent(channel)+
                        mGamma[4] * allPassFilterBank[3].getDelayedComponent(channel)+
                        mGamma[5] * allPassFilterBank[4].getDelayedComponent(channel)+
                        allPassFilterBank[5].getDelayedComponent(channel);


            float inputSample = alpha0 * (inputBlock[sample] - mK * Sn);
            /*float inputSample = inputBlock[sample];*/
            float processedSample = 0.f;

            //===========================================================================
            for (int i = 0; i < numFilter; ++i)
            {
                processedSample = allPassFilterBank[i].processSample(inputSample, channel);
                inputSample = processedSample;
            }

            outputBlock[sample] = 0.707 * inputSample * (1 - (phaserParams.phs_mix / 100.0)) + 0.707 * processedSample * (phaserParams.phs_mix / 100.0);
        }
        
        DBG(phaserParams.phs_Rate);
    }
}
    

void NSCPhaser::setParameters(const int channel)
{
    //======Set LFO parameters======================
    setLFOParameters(channel);


    //======Set All pass filter parameters======================
    setAllPassFilterParameters(channel);
}

float NSCPhaser::returnModulatedFc(double modulatorValue, double minFc, double maxFc)
{
    auto halfLength = (maxFc - minFc) / 2.0;
    auto midPoint = minFc + halfLength;
    return midPoint + modulatorValue * halfLength;
}


void  NSCPhaser::setLFOParameters(const int channel)
{

        auto LFOparams = mLFOs[channel].getParameters();
        LFOparams.mWaveform = phaserParams.lfoParams.mWaveform;
        LFOparams.oscFrequency = phaserParams.phs_Rate;
        mLFOs[channel].setParameters(LFOparams);


}

void  NSCPhaser::setAllPassFilterParameters(const int channel)
{
    auto lfoRawValue = mLFOs[channel].getValue();
    double mDepth = phaserParams.phs_Depth / 100.0;
    modulatorValue = lfoRawValue * mDepth;

    
    for (int i = 0; i < numFilter; ++i)
    {
        auto parameters = allPassFilterBank[i].getParameters();
        auto filterFc = returnModulatedFc(modulatorValue, minFc[i], maxFc[i]);
        parameters.mfc = filterFc;
        //mQ[i] = filterFc / notchWidth;
        parameters.mQ = mQ[i];
        allPassFilterBank[i].setParameters(parameters);
        
    }
}

void NSCPhaser::setPhaserParameters(const PhaserParameters& updatedParams)
{
    phaserParams.phs_Depth = updatedParams.phs_Depth;
    phaserParams.phs_Feed = updatedParams.phs_Feed;
    phaserParams.phs_mix = updatedParams.phs_mix;
    phaserParams.phs_Rate = updatedParams.phs_Rate;
    phaserParams.phs_Stereo=updatedParams.phs_Stereo;
    phaserParams.lfoParams.mWaveform = updatedParams.lfoParams.mWaveform;
}