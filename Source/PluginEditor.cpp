/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FMFunEditor::FMFunEditor (FMFun& p,APVTS& apvts)
    : AudioProcessorEditor (&p), audioProcessor (p),apvts(apvts)
{
  

    addAndMakeVisible (mISlider);
    // This idiom i am not super happy with.
    mIAttachment.reset (new SliderAttachment (apvts, "mI", mISlider));
   
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
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
}
