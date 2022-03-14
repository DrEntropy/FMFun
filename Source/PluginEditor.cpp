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
      modControl("mod",apvts),filterControl("filter",apvts),pitchControl("pitch",apvts),
      mISlider("mI",apvts),ratioSlider("Ratio", apvts), cutOffSlider("cutOff", apvts),
      pitchModSlider("pitchMod",apvts),filterModSlider("filterMod",apvts),
      fbSlider("fb",apvts)
{
  

    addAndMakeVisible (mISlider);
    addAndMakeVisible (ratioSlider);
    addAndMakeVisible (cutOffSlider);
    addAndMakeVisible  (pitchModSlider);
    addAndMakeVisible  (filterModSlider);
    addAndMakeVisible (fbSlider);

   
    
    addAndMakeVisible(ampControl);
    addAndMakeVisible(modControl);
    addAndMakeVisible(filterControl);
    addAndMakeVisible(pitchControl);
    // This idiom i am not super happy with.
    //  mIAttachment.reset (new SliderAttachment (apvts, "mI", mISlider));
   
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
 
    
    auto bounds = getLocalBounds();
    // top is for the regular sliders, tbd
    auto sliderBounds = bounds.removeFromTop(getHeight()/2);
   
    
    // six sliders : index, Ratio, Cutoff, pitchMod amount, filter Mod amount, feedback amount.
    int sliderH = sliderBounds.getHeight()/6;
    mISlider.setBounds(sliderBounds.removeFromTop (sliderH));
    ratioSlider.setBounds(sliderBounds.removeFromTop (sliderH));
    cutOffSlider.setBounds(sliderBounds.removeFromTop (sliderH));
    pitchModSlider.setBounds(sliderBounds.removeFromTop (sliderH));
    filterModSlider.setBounds(sliderBounds.removeFromTop (sliderH));
    fbSlider.setBounds(sliderBounds);
    
    // room for one more here
     

  
    
    // set bounds for the ADSRs on the bottom half
   int adsrH = bounds.getHeight()/4;
    
    ampControl.setBounds (bounds.removeFromTop(adsrH));
    modControl.setBounds (bounds.removeFromTop(adsrH));
    pitchControl.setBounds (bounds.removeFromTop(adsrH));
    filterControl.setBounds (bounds.removeFromTop(adsrH));
    
}
