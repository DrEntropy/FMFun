/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FMFunEditor::FMFunEditor (FMFun& p,APVTS& apvts)
    : AudioProcessorEditor (&p), audioProcessor (p),apvts(apvts),ampControl("amp",apvts),
      modControl("mod",apvts),filterControl("filter",apvts),pitchControl("pitch",apvts)
{
  

    addAndMakeVisible (mISlider);
    addAndMakeVisible(ampControl);
    addAndMakeVisible(modControl);
    addAndMakeVisible(filterControl);
    addAndMakeVisible(pitchControl);
    // This idiom i am not super happy with.
    mIAttachment.reset (new SliderAttachment (apvts, "mI", mISlider));
   
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 800);
}

FMFunEditor::~FMFunEditor()
{
}

//==============================================================================
void FMFunEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
   // g.drawFittedText ("No gui yet!", getLocalBounds(), juce::Justification::centred, 1);
}

void FMFunEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    mISlider.setBounds(getLocalBounds());
    
    auto bounds = getLocalBounds();
    // top is for the regular sliders, tbd
    mISlider.setBounds(bounds.removeFromTop (getHeight() / 2));
    
    // set bounds for the ADSRs on the bottom half
   int adsrH = bounds.getHeight()/4;
    
    ampControl.setBounds (bounds.removeFromTop(adsrH));
    modControl.setBounds (bounds.removeFromTop(adsrH));
    pitchControl.setBounds (bounds.removeFromTop(adsrH));
    filterControl.setBounds (bounds.removeFromTop(adsrH));
    
}
