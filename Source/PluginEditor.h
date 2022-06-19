/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ADSR.h"

//==
class ParameterSlider : public juce::Component
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    ParameterSlider (const juce::String& parameterID,APVTS& apvts) : apvts(apvts)
    {
        addAndMakeVisible (theSlider);
        theAttachment.reset (new SliderAttachment (apvts,parameterID, theSlider));
        // 10 characters? To do make this better.
        auto name = apvts.getParameter(parameterID)->getName(10);
        addAndMakeVisible (theLabel);
        theLabel.setText (name, juce::dontSendNotification);
        theLabel.attachToComponent (&theSlider, true);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        auto width = getWidth();
        
        bounds.removeFromLeft(width/6); // make room for the labels.
        theSlider.setBounds(bounds);
        
        
    }
    
private:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    APVTS& apvts;
    
    juce::Slider theSlider;
    juce::Label theLabel;
    std::unique_ptr<SliderAttachment> theAttachment;
    
};


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
    
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    // Amplitude envelopes.
    ADSRControl ampControl;
    ADSRControl modControl;
    ADSRControl filterControl;
    ADSRControl pitchControl;
    
    // define the controls.... will be so many!
    ParameterSlider mISlider;
    juce::Slider mixSlider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
    std::unique_ptr<SliderAttachment> mixAttachment;
   
    ParameterSlider ratioSlider;
    ParameterSlider detuneSlider;
    ParameterSlider cutOffSlider;
    ParameterSlider resSlider;
    ParameterSlider pitchModSlider;
    ParameterSlider filterModSlider;
    ParameterSlider fbSlider;
    ParameterSlider fbSlider2;  // for parallel mode only?
    
    // Parallel mode
    juce::ToggleButton parallelMode{"Parallel"};
    std::unique_ptr<ButtonAttachment> parellelModeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FMFunEditor)
};
