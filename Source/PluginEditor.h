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
// just starting to figure this out.
// plan is for each ADSR to have a ref to the APVTS and make necessary attachments.

class ADSRControl : public juce::GroupComponent
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    ADSRControl (const juce::String& name,APVTS& apvts) : apvts(apvts)
    {
        // later add seperate display name
        
        setText (name);
        setTextLabelPosition (juce::Justification::centredTop);
        addAndMakeVisible (A_Slider);
        addAndMakeVisible (D_Slider);
        addAndMakeVisible (S_Slider);
        addAndMakeVisible (R_Slider);
        A_Attachment.reset (new SliderAttachment (apvts, "A_"+name, A_Slider));
        D_Attachment.reset (new SliderAttachment (apvts, "D_"+name, D_Slider));
        S_Attachment.reset (new SliderAttachment (apvts, "S_"+name, S_Slider));
        R_Attachment.reset (new SliderAttachment (apvts, "R_"+name, R_Slider));
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        auto width = getWidth();
        A_Slider.setBounds (bounds.removeFromLeft(width/4.0));
        D_Slider.setBounds (bounds.removeFromLeft(width/4.0));
        S_Slider.setBounds (bounds.removeFromLeft(width/4.0));
        R_Slider.setBounds (bounds);
        
    }
    
    juce::Slider A_Slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider D_Slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider S_Slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider R_Slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
private:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    APVTS& apvts;
    std::unique_ptr<SliderAttachment> A_Attachment;
    std::unique_ptr<SliderAttachment> D_Attachment;
    std::unique_ptr<SliderAttachment> S_Attachment;
    std::unique_ptr<SliderAttachment> R_Attachment;
};


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
    
    
    // Amplitude envelopes.
    ADSRControl ampControl;
    ADSRControl modControl;
    ADSRControl filterControl;
    ADSRControl pitchControl;
    
    // define the controls.... will be so many!
    ParameterSlider op2AmpSlider;
    ParameterSlider ratioSlider;
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
