/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FMFunEditor::FMFunEditor (FMFun& p,APVTS& apvts)
    : AudioProcessorEditor (&p), audioProcessor (p),apvts(apvts),ampControl("op1EG",apvts),
      modControl("op2EG",apvts),filterControl("filter",apvts),pitchControl("pitch",apvts),
      mISlider("mI",apvts),ratioSlider("Ratio", apvts),detuneSlider("Detune",apvts),
      cutOffSlider("cutOff", apvts),resSlider("res",apvts),
      pitchModSlider("pitchMod",apvts),filterModSlider("filterMod",apvts),
      fbSlider("fb",apvts),fbSlider2("fb2",apvts)
{
  

    addAndMakeVisible (mISlider);
    
    // this one is rotary so seperated out.
    addAndMakeVisible (mixSlider);
    mixAttachment.reset(new SliderAttachment(apvts, "opMix", mixSlider));
    
    addAndMakeVisible (ratioSlider);
    addAndMakeVisible (detuneSlider);
    addAndMakeVisible (cutOffSlider);
    addAndMakeVisible (resSlider);
    addAndMakeVisible (pitchModSlider);
    addAndMakeVisible (filterModSlider);
    addAndMakeVisible (fbSlider);
    addAndMakeVisible (fbSlider2);
    
    //button for running the two ops in parallel instead.
    addAndMakeVisible (parallelMode);
    parellelModeAttachment.reset(new ButtonAttachment(apvts, "pMode", parallelMode));
    
    addAndMakeVisible(ampControl);
    addAndMakeVisible(modControl);
    addAndMakeVisible(filterControl);
    addAndMakeVisible(pitchControl);
   
   
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 800);
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
    // split in two
    // TODO rmeove hard wired numbers
    auto rightBounds = bounds.removeFromRight(100);
    parallelMode.setBounds(rightBounds.removeFromTop(100));
    mixSlider.setBounds(rightBounds.removeFromTop(100));
    // top is for the regular sliders, tbd
    auto sliderBounds = bounds.removeFromTop(getHeight()/2);
   
    
    const int num_of_sliders = 9;
    int sliderH = sliderBounds.getHeight() / num_of_sliders;
    mISlider.setBounds(sliderBounds.removeFromTop (sliderH));
    
    ratioSlider.setBounds(sliderBounds.removeFromTop(sliderH));
    detuneSlider.setBounds(sliderBounds.removeFromTop(sliderH));
    fbSlider.setBounds(sliderBounds.removeFromTop(sliderH));
    fbSlider2.setBounds(sliderBounds.removeFromTop(sliderH));
    cutOffSlider.setBounds(sliderBounds.removeFromTop (sliderH));
    resSlider.setBounds(sliderBounds.removeFromTop(sliderH));
    pitchModSlider.setBounds(sliderBounds.removeFromTop(sliderH));
    filterModSlider.setBounds(sliderBounds);

    
    
    // room for one more here
     

  
    
    // set bounds for the ADSRs on the bottom half
   int adsrH = bounds.getHeight()/4;
    
    ampControl.setBounds (bounds.removeFromTop(adsrH));
    modControl.setBounds (bounds.removeFromTop(adsrH));
    pitchControl.setBounds (bounds.removeFromTop(adsrH));
    filterControl.setBounds (bounds.removeFromTop(adsrH));
    
}
