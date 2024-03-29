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
        pMode  = apvts.getRawParameterValue("pMode")-> load();
    }
    
    void setCurrentPlaybackSampleRate (double newRate) override {
        // not sure this is the best place for this .
        // hardwired 50 ms transition rate.
        // this should be called at start up
       //filter.prepare({newRate,ap.getMaximumBlockSize(),ap.getMainBusNumOutputChannels()});
        
        if(newRate>0)
        {
          s_mI.reset(newRate, 0.050f);
            op1Env.setSampleRate(newRate);
            op2Env.setSampleRate(newRate);
            pitchEnv.setSampleRate(newRate);
            filterEnv.setSampleRate(newRate);
            
            setupEnvelopes();
          
        // op2Env.setParameters(juce::ADSR::Parameters(.1f,.1f,1.0f,.1f));
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
      
        prevSample1=prevSample2 = 0.0;
        level = velocity * 0.15;
//        tailOff = 0.0;
        
        cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        // base frequency
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
        
       
        
        // get env parameters and reset them.
  
        setupEnvelopes();
        
        // Start envelopes running
        
        
        op1Env.noteOn();
        op2Env.noteOn();
        pitchEnv.noteOn();
        filterEnv.noteOn();
         
        
        // Jump smoothed parameters to current value
        s_mI.setCurrentAndTargetValue(apvts.getRawParameterValue("mI")->load());
        
        float filterMod= apvts.getRawParameterValue("filterMod")->load();
        // start the filter in the right state ...
        if(filterMod <= 0)
             filter.setCutoffFrequencyHz(apvts.getRawParameterValue("cutOff")->load());
        else
            filter.setCutoffFrequencyHz(20.0f);
        filter.reset();
        //filter.setCutoffFrequencyHz(1000.0f);
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            op1Env.noteOff();
            op2Env.noteOff();
            filterEnv.noteOff();
            pitchEnv.noteOff();
//            if (tailOff == 0.0)
//                tailOff = 1.0;
        }
        else
        {
            op1Env.reset();
            op2Env.reset();
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
             

            // see https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
            // mI because op2 amp is also modulation index
            float newmI = apvts.getRawParameterValue("mI")->load();
            s_mI.setTargetValue(newmI);
            
            
            if (op1Env.isActive() || (op2Env.isActive() && pMode))
            {
                float pitchModAmt =  apvts.getRawParameterValue("pitchMod")->load();
                float filterModAmt = apvts.getRawParameterValue("filterMod")->load();
                float modRatio = apvts.getRawParameterValue("Ratio")->load();
                float mix = apvts.getRawParameterValue("opMix")-> load();
                float mix1 =  sqrt((1.0-mix)/2.0);
                float mix2 = sqrt((1.0+mix)/2.0);
                
                // note no GUI for this one yet.
                float detune = apvts.getRawParameterValue("Detune")-> load();
                
                // not used yet, will use prevSample member variable
                float fb1 = apvts.getRawParameterValue("fb")-> load();
                float fb2  = apvts.getRawParameterValue("fb2")-> load();
                
                // set pMode
                pMode  = apvts.getRawParameterValue("pMode")-> load();
                
                
                auto numChannels = outputBuffer.getNumChannels();
                
                
                // generate the fm wave in a temporary buffer prior to DSP processing.
                // one channel only
                juce::AudioBuffer<float> tempBuff(1,numSamples);
                auto currSampleNum = 0;
                auto sampleCount = numSamples;
                while (--sampleCount >= 0)
                {
                    if(sampleCount==numSamples-1)
                    {
                        // once per block
                        
                        // I might want a smoother for pitchModAmt and filter mod amt
                        
                        // compute filter mod.
                        float filterMod;
                        // this bit here is not tested yest
                        // filter mod amt determins how deep the filtering goes down
                        if(filterModAmt >= 0)
                            filterMod = (filterEnv.getNextSample() - 1.0f) * filterModAmt;
                        else
                            filterMod = filterEnv.getNextSample() * filterModAmt;
                        
                        // note these should always be negative ^^^
                        
                        // cutOff now sets maximum
                        float cutOffSetting =  apvts.getRawParameterValue("cutOff")->load();
                        
                        //
                        
                        float cutOff = exp2(filterMod)*cutOffSetting;
                        
                        
                        // note that the ladder filter has built in .05 smoothing ...its in the source.
                        // NOTE: Might need to jlimit this to make sure it is in range later ...
                        filter.setCutoffFrequencyHz(cutOff);
                        float resonance =apvts.getRawParameterValue("res")->load();
                        filter.setResonance(resonance);
                        
                    } else {
                        // advance the filter envelope.
                         
                        filterEnv.getNextSample();
                    }
               
                    // maybe only do this once per block??? Consider, do as i did the filter.
                    float pitchMod = pitchModAmt*pitchEnv.getNextSample();
                    float currAngleDelta = angleDelta * exp2(pitchMod); // max mod is an octave for now.change this to use left shift?
                    float currentAngle2 =currentAngle * modRatio * exp2(detune/1200.0); // detune
                    float currentSample;
                    float mI = s_mI.getNextValue();
                    
                    // This is where the actual Phase modulation take place
                    //
                    
                    if(pMode)
                    {
                        prevSample1  = (float) (std::sin(currentAngle  + fb1 * prevSample1));
                        prevSample2  = (float) (std::sin(currentAngle2 + fb2 * prevSample2));
                        currentSample = (mix1 * prevSample1 * op1Env.getNextSample()+mix2 * op2Env.getNextSample() * prevSample2) * level;
                    } else {
                    
                        prevSample1 = (float) (std::sin (currentAngle + op2Env.getNextSample() *
                                                mI * std::sin(currentAngle2)) * level * op1Env.getNextSample());
                        prevSample2 = 0.0f;
                        currentSample = prevSample1;
                    }
                    
                 
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
        setEnvelope(op1Env,"op1EG");
        setEnvelope(op2Env,"op2EG");
        setEnvelope(pitchEnv,"pitch");
        setEnvelope(filterEnv,"filter");
        // reset the envelopes in case we got stolen note. Should happen with stopnote,
        // but without this i noted that the envelope does not reset when playing notes rapidly.
        op1Env.reset(); op2Env.reset();pitchEnv.reset(); filterEnv.reset();
    }
    void setEnvelope(juce::ADSR& env,std::string param_name){
       
        // this is a bit of a hack, and not really a safe way to do this.
        // maybe better to make a subclass of ADSR that include refs to the parameters
 
         env.setParameters(juce::ADSR::Parameters(apvts.getRawParameterValue("A_"+param_name)->load(),
                                                    apvts.getRawParameterValue("D_"+param_name)->load(),
                                                    apvts.getRawParameterValue("S_"+param_name)->load(),
                                                    apvts.getRawParameterValue("R_"+param_name)->load()));
    }
    
    
//    void setModEnvelop(){
//       // apvts.getRawParameterValue("A_amp")->load();
//        op2Env.setParameters(juce::ADSR::Parameters(apvts.getRawParameterValue("A_mod")->load(),
//                                                    apvts.getRawParameterValue("D_mod")->load(),
//                                                    apvts.getRawParameterValue("S_mod")->load(),
//                                                    apvts.getRawParameterValue("R_mod")->load()));
//    }
    
    double currentAngle = 0.0,angleDelta = 0.0, level = 0.0;
    float prevSample1 =0.0f;  // Used for feedback FM
    float prevSample2 =0.0f;  // Used for feedback FM parallel mode
//    double tailOff = 0.0;
    
    double pitchShift =0.0;
    double cyclesPerSecond=0.0; // current frequency
    
    bool pMode =false;
    juce::AudioProcessorValueTreeState& apvts;
    
    
    //smothing variables starts at zero
    juce::SmoothedValue<float> s_mI{0.f};
    
    
    // ladder filter for each voice
     juce::dsp::LadderFilter<float> filter{};
    
    juce::ADSR op1Env{};
    juce::ADSR op2Env{};
    juce::ADSR filterEnv{};
    juce::ADSR pitchEnv{};
};


