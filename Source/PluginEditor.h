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
class FMFunEditor  : public juce::AudioProcessorEditor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    FMFunEditor (FMFun&,APVTS&);
    ~FMFunEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FMFun& audioProcessor;
    

    
    APVTS& apvts;
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    
    // define the controls.... will be so many!
    juce::Slider mISlider;
    std::unique_ptr<SliderAttachment> mIAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FMFunEditor)
};
