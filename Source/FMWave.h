/*
  ==============================================================================

    FMWave.h
     Based on SineWave.h from Juce Team.

  ==============================================================================
*/

#pragma once
//==============================================================================
struct FMSound   : public juce::SynthesiserSound
{
    FMSound() {}

    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};

//==============================================================================
struct FMVoice   : public juce::SynthesiserVoice
{
    FMVoice(juce::AudioProcessorValueTreeState& apvts):apvts(apvts) {
        
   
    }
    
    void setCurrentPlaybackSampleRate (double newRate) override {
        // not sure this is the best place for this .
        // hardwired 50 ms transition rate.
        
        if(newRate>0)
        {
          s_mI.reset(newRate,0.050f);
            ampEnv.setSampleRate(newRate);
            modEnv.setSampleRate(newRate);
         ampEnv.setParameters(juce::ADSR::Parameters(.01f,.1f,0.0f,.1f));
         modEnv.setParameters(juce::ADSR::Parameters(.1f,.1f,1.0f,.1f));
        }
        // call super
        juce::SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<FMSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
//        tailOff = 0.0;
      

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
        ampEnv.noteOn();
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            ampEnv.noteOff();
//            if (tailOff == 0.0)
//                tailOff = 1.0;
        }
        else
        {
            ampEnv.reset();
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int) override      {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            // TODO, smooth this out.
            // see https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
            float newmI = apvts.getRawParameterValue("mI")->load();
          
            s_mI.setTargetValue(newmI);
            float mI;
            
            if (ampEnv.isActive()) // [7]
            {
               
                while (--numSamples >= 0)
                {
                    mI = s_mI.getNextValue();
                    auto currentSample = (float) (std::sin (currentAngle + mI*std::sin(currentAngle)) * level * ampEnv.getNextSample());

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

              //      tailOff *= 0.99; // [8]

//                    if (tailOff <= 0.005)
//                    {
//                        clearCurrentNote(); // [9]
//
//                        angleDelta = 0.0;
//                        break;
//                    }
                }
            }
            else
            {
                clearCurrentNote(); // [9]

                angleDelta = 0.0;
//                while (--numSamples >= 0) // [6]
//                {
//                    mI = s_mI.getNextValue();
//                    auto currentSample = (float) (std::sin (currentAngle+ mI*std::sin(currentAngle)) * level);
//
//                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
//                        outputBuffer.addSample (i, startSample, currentSample);
//
//                    currentAngle += angleDelta;
//                    ++startSample;
//                }
            }
        }
    }

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
//    double tailOff = 0.0;
    
    juce::AudioProcessorValueTreeState& apvts;
    
    //smothing variables starts at zero
    juce::SmoothedValue<float> s_mI{0.f};
    
    juce::ADSR ampEnv{};
    juce::ADSR modEnv{};
};


