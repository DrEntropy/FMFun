/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void addEnvelope(juce::AudioProcessorValueTreeState::ParameterLayout& layout,std::string name){
    layout.add(std::make_unique<juce::AudioParameterFloat> ("A_"+name,"A-"+name,juce::NormalisableRange<float>(0,1),0));
    layout.add(std::make_unique<juce::AudioParameterFloat> ("D_"+name,"D-"+name,juce::NormalisableRange<float>(0,1),0));
    layout.add(std::make_unique<juce::AudioParameterFloat> ("S_"+name,"S-"+name,juce::NormalisableRange<float>(0,1),0));
    layout.add(std::make_unique<juce::AudioParameterFloat> ("R_"+name,"R-"+name,juce::NormalisableRange<float>(0,1),0));
}
 
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    
    
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
     
  
    
    layout.add(std::make_unique<juce::AudioParameterFloat> ("mI","Index",juce::NormalisableRange<float>(0,10),0));
    layout.add(std::make_unique<juce::AudioParameterFloat> ("Ratio","Ratio",juce::NormalisableRange<float>(0.0,10.0),1.0));
    layout.add(std::make_unique<juce::AudioParameterFloat> ("cutOff","cutOff",juce::NormalisableRange<float>(20.f,20000.f,0,0.2f),1000.f));
    // four octaves, note that at extremes there will be aliasing and it sound funky as f.
    layout.add(std::make_unique<juce::AudioParameterFloat> ("pitchMod","pitch mod",juce::NormalisableRange<float>(-4.0f,4.0f),0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat> ("filterMod","filter mod",juce::NormalisableRange<float>(-10.0f,10.0f),0.f));
    // UNTESTED
    layout.add(std::make_unique<juce::AudioParameterFloat> ("res","Resonance",juce::NormalisableRange<float>(0.0f,1.0f),0.f));
    // Feedback doesnt do anything yet
    layout.add(std::make_unique<juce::AudioParameterFloat> ("fb","Feedback",juce::NormalisableRange<float>(0.0f,10.0f),0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat> ("fb2","Feedback 2",juce::NormalisableRange<float>(0.0f,10.0f),0.f));
    
    layout.add(std::make_unique<juce::AudioParameterBool> ("pMode","Parallel Mode",false));
               
    // Amplitude envelope
    addEnvelope(layout,"amp");
    //modulator envelope 
    addEnvelope(layout,"mod");
    // filter envelope
    addEnvelope(layout,"filter");
    // pitch envelope
    addEnvelope(layout,"pitch");
 
    return layout;
}

//==============================================================================
FMFun::FMFun()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
, apvts( *this, nullptr, "PARAMETERS",createParameterLayout())
{
    
    for (auto i = 0; i < 6; ++i)
        synth.addVoice (new FMVoice(apvts));

    synth.addSound (new FMSound());
}

FMFun::~FMFun()
{
}

//==============================================================================
const juce::String FMFun::getName() const
{
    return JucePlugin_Name;
}

bool FMFun::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FMFun::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FMFun::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FMFun::getTailLengthSeconds() const
{
    return 0.0;
}

int FMFun::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FMFun::getCurrentProgram()
{
    return 0;
}

void FMFun::setCurrentProgram (int index)
{
}

const juce::String FMFun::getProgramName (int index)
{
    return {};
}

void FMFun::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FMFun::prepareToPlay ( double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // prepare the voices, mostly the dsp. I am not a big fan of using the cast here,
    // TODO: Make a proper synth subclass to handle this wihtout a cast.
    for(int i = 0; i< synth.getNumVoices();++i)
    {
        // for now i hardwire the output channels to 1 because the synth is mono internally for now.
        //static_cast<juce::uint32>( getMainBusNumOutputChannels())
        juce::dsp::ProcessSpec spec{static_cast<float>(sampleRate),static_cast<juce::uint32>(samplesPerBlock),1};
        static_cast<FMVoice*>(synth.getVoice(i))->prepare(spec);
    }
    
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void FMFun::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FMFun::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FMFun::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
 
      
    synth.renderNextBlock (buffer, midiMessages,
                            0, buffer.getNumSamples());
    
}

//==============================================================================
bool FMFun::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FMFun::createEditor()
{
    return new FMFunEditor (*this, apvts);
}
 // these are copied from teh juce tutorial.
//==============================================================================
void FMFun::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void FMFun::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
           if (xmlState->hasTagName (apvts.state.getType()))
               apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FMFun();
}
