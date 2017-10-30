/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/

class GLITCH_DELAY_EFFECT;

class GlitchDelayPluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    GlitchDelayPluginAudioProcessor();
    ~GlitchDelayPluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    
    static float                                                MAX_FEEDBACK;
    
    std::unique_ptr<GLITCH_DELAY_EFFECT>                        m_effect;
    AudioSampleBuffer                                           m_prev_buffer;
    
    AudioParameterFloat*                                        m_mix;
    AudioParameterFloat*                                        m_feedback;
	
	AudioParameterBool*											m_freeze_active;
	
	AudioParameterFloat*                                        m_low_head_mix;
    AudioParameterFloat*                                        m_low_head_size;
	AudioParameterFloat*                                        m_low_head_jitter;
	
	AudioParameterFloat*                                        m_normal_head_mix;
	AudioParameterFloat*                                        m_normal_head_size;
	AudioParameterFloat*                                        m_normal_head_jitter;
	
	AudioParameterFloat*                                        m_high_head_mix;
	AudioParameterFloat*                                        m_high_head_size;
	AudioParameterFloat*                                        m_high_head_jitter;
	
    AudioParameterFloat*                                        m_reverse_head_mix;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlitchDelayPluginAudioProcessor)
};
