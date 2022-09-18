/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Lavno_PhaserAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Lavno_PhaserAudioProcessorEditor (Lavno_PhaserAudioProcessor&);
    ~Lavno_PhaserAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Lavno_PhaserAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Lavno_PhaserAudioProcessorEditor)
};
