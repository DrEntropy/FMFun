/*
  ==============================================================================

    ADSR.h
    Created: 19 Jun 2022 10:04:39am
    Author:  Ronald Legere

  ==============================================================================
*/

#pragma once
 
#include <JuceHeader.h>

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

