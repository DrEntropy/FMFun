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
    
    // for dsp
    
    void prepare(const juce::dsp::ProcessSpec &spec) {
        //
        filter.prepare(spec);
        filter.setMode(juce::dsp::LadderFilterMode::LPF24);
    }
    
    void setCurrentPlaybackSampleRate (double newRate) override {
        // not sure this is the best place for this .
        // hardwired 50 ms transition rate.
        // this should be called at start up
       //filter.prepare({newRate,ap.getMaximumBlockSize(),ap.getMainBusNumOutputChannels()});
        
        if(newRate>0)
        {
          s_mI.reset(newRate,0.050f);
            ampEnv.setSampleRate(newRate);
            modEnv.setSampleRate(newRate);
            pitchEnv.setSampleRate(newRate);
            filterEnv.setSampleRate(newRate);
            
            setupEnvelopes();
          
        // modEnv.setParameters(juce::ADSR::Parameters(.1f,.1f,1.0f,.1f));
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
        
        cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        // base frequency
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
        
       
        
        // get env parameters and reset them.
  
        setupEnvelopes();
        
        // Start envelopes running
        
        
        ampEnv.noteOn();
        modEnv.noteOn();
        pitchEnv.noteOn();
        filterEnv.noteOn();
         
        
        // Jump smoothed parameters to current value
        s_mI.setCurrentAndTargetValue(apvts.getRawParameterValue("mI")->load());
        filter.setCutoffFrequencyHz(apvts.getRawParameterValue("cutOff")->load());
        filter.reset();
        //filter.setCutoffFrequencyHz(1000.0f);
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            ampEnv.noteOff();
            modEnv.noteOff();
            filterEnv.noteOff();
            pitchEnv.noteOff();
//            if (tailOff == 0.0)
//                tailOff = 1.0;
        }
        else
        {
            ampEnv.reset();
            modEnv.reset();
            pitchEnv.reset();
            filterEnv.reset();
            clearCurrentNote();
            filter.reset();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int value) override      {
        pitchShift = value/128.0;
        //
    }
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
             
            // I might want a smoother for pitchModAmt and filter mod amt
            
            float pitchModAmt =  apvts.getRawParameterValue("pitchMod")->load();
            float filterModAmt = apvts.getRawParameterValue("filterMod")->load();
        
            // see https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
            float newmI = apvts.getRawParameterValue("mI")->load();
            s_mI.setTargetValue(newmI);
            
            // compute filter mod.
            float filterMod = filterEnv.getNextSample()*filterModAmt;
            float cutOffSetting =  apvts.getRawParameterValue("cutOff")->load();
            
            float cutOff = juce::jlimit(20.0,20000.0,cutOffSetting+filterMod*20000.0);
            // note that the latter filter has built in .05 smoothing ...its in the source.
            filter.setCutoffFrequencyHz(cutOff);
          
            float mI;
            
            if (ampEnv.isActive())
            {
                auto numChannels = outputBuffer.getNumChannels();
                
                
                // generate the fm wave into a temporary buffer prior to DSP processing.
                // one channel only
                juce::AudioBuffer<float> tempBuff(1,numSamples);
                auto currSampleNum = 0;
                auto sampleCount = numSamples;
                while (--sampleCount >= 0)
                {
                    // advance the filter envelope.
                    // TODO fix the off by one sample issue here. or not.
                    filterEnv.getNextSample();
                    // maybe only do this once per block??? Consider, do as i did the filter.
                    float pitchMod = pitchModAmt*pitchEnv.getNextSample();
                    float currAngleDelta = angleDelta * pow(2.0f,pitchMod); // max mod is an octave for now.change this to use left shift?
                    mI = s_mI.getNextValue();
                    auto currentSample = (float) (std::sin (currentAngle + modEnv.getNextSample() *
                                                mI * std::sin(currentAngle)) * level * ampEnv.getNextSample());
                    tempBuff.setSample(0, currSampleNum, currentSample);

                    currentAngle += currAngleDelta;
                    ++currSampleNum;

 
                }
                // filter processing - single channel
                // basically copied out of the tutorial after trying for about 1 hour to figure it out
                // from the juce documentation.  Why not have a consistent interface?
                auto block = juce::dsp::AudioBlock<float> (tempBuff);
                 // this mightbe superflous at this point.
                //auto blockToUse = block.getSubBlock ((size_t) 0, (size_t) numSamples);
                auto contextToUse = juce::dsp::ProcessContextReplacing<float> (block);
                
               filter.process(contextToUse);
                
                // add result to output buffer 
                
                for (auto i = numChannels;--i >= 0;)
                    outputBuffer.addFrom(i, startSample, tempBuff, 0, 0, numSamples);
 
            }
            else
            {
                clearCurrentNote();
                //not sure this is needed.
                filter.reset();

                angleDelta = 0.0;
 
            }
        }
    }

private:
    
    void setupEnvelopes(){
        setEnvelope(ampEnv,"amp");
        setEnvelope(modEnv,"mod");
        setEnvelope(pitchEnv,"pitch");
        setEnvelope(filterEnv,"filter");
        // reset the envelopes in case we got stolen note. Should happen with stopnote,
        // but without this i noted that the envelope does not reset when playing notes rapidly.
        ampEnv.reset(); modEnv.reset();pitchEnv.reset(); filterEnv.reset();
    }
    void setEnvelope(juce::ADSR& env,std::string name){
       
        // this is a bit of a hack, and not really a safe way to do this.
        // maybe better to make a subclass of ADSR that include refs to the parameters
 
         env.setParameters(juce::ADSR::Parameters(apvts.getRawParameterValue("A_"+name)->load(),
                                                    apvts.getRawParameterValue("D_"+name)->load(),
                                                    apvts.getRawParameterValue("S_"+name)->load(),
                                                    apvts.getRawParameterValue("R_"+name)->load()));
    }
    
    
//    void setModEnvelop(){
//       // apvts.getRawParameterValue("A_amp")->load();
//        modEnv.setParameters(juce::ADSR::Parameters(apvts.getRawParameterValue("A_mod")->load(),
//                                                    apvts.getRawParameterValue("D_mod")->load(),
//                                                    apvts.getRawParameterValue("S_mod")->load(),
//                                                    apvts.getRawParameterValue("R_mod")->load()));
//    }
    
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
//    double tailOff = 0.0;
    
    double pitchShift =0.0;
    double cyclesPerSecond=0.0; // current frequency
    
    
    juce::AudioProcessorValueTreeState& apvts;
    
    
    //smothing variables starts at zero
    juce::SmoothedValue<float> s_mI{0.f};
    
    
    // ladder filter for each voice
     juce::dsp::LadderFilter<float> filter{};
    
    juce::ADSR ampEnv{};
    juce::ADSR modEnv{};
    juce::ADSR filterEnv{};
    juce::ADSR pitchEnv{};
};


