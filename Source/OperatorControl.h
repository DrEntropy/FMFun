/*
  ==============================================================================

    OperatorControl.h
    Created: 19 Jun 2022 10:10:16am
    Author:  Ronald Legere

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OperatorControl  : public juce::Component
{
public:
    OperatorControl();
    ~OperatorControl() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OperatorControl)
};
