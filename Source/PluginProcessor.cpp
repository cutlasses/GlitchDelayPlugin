/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <array>

#include "JuceHeader.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "GlitchDelayEffect.h"

namespace
{
    // mixdown N channels of buffer into channel 0, using the weights array
    template< size_t N >
    void mix_down( AudioSampleBuffer& buffer, std::array<float, N > weights )
    {
        jassert( buffer.getNumChannels() == N );
        
        for( int s = 0; s < buffer.getNumSamples(); ++s )
        {
            float mixed = 0.0f;
            for( int c = 0; c < buffer.getNumChannels(); ++c )
            {
                mixed += buffer.getSample( c, s ) * weights[ c ];
            }
            
            buffer.setSample( 0, s, mixed );
        }
    }
    
    // mix buffer b2 into b1 using weights w1, w2
    void mix_into( AudioSampleBuffer& b1, AudioSampleBuffer& b2, int channel, float w1, float w2 )
    {
        jassert( b1.getNumChannels() > channel && b2.getNumChannels() > channel );
        
        for( int s = 0; s < b1.getNumSamples(); ++s )
        {
            float mixed = ( b1.getSample( channel, s ) * w1 ) + ( b2.getSample( channel, s ) * w2 );
            
            b1.setSample( channel, s, mixed );
        }
    }
	
	enum PLAY_HEAD_INDICES
	{
		LOW_HEAD = 0,
		NORMAL_HEAD = 1,
		HIGH_HEAD = 2,
		REVERSE_HEAD = 4
	};
}

//==============================================================================

float GlitchDelayPluginAudioProcessor::MAX_FEEDBACK = 0.85f;

GlitchDelayPluginAudioProcessor::GlitchDelayPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
	m_mix( nullptr ),
	m_feedback( nullptr ),
	m_freeze_active( nullptr ),
	m_low_head_mix( nullptr ),
	m_low_head_size( nullptr ),
	m_low_head_jitter( nullptr ),
	m_normal_head_mix( nullptr ),
	m_normal_head_size( nullptr ),
	m_normal_head_jitter( nullptr ),
	m_high_head_mix( nullptr ),
	m_high_head_size( nullptr ),
	m_high_head_jitter( nullptr ),
	m_reverse_head_mix( nullptr )
#endif
{
    // create the wrapped effect
    m_effect = make_unique< GLITCH_DELAY_EFFECT >();
    
    addParameter( m_mix = new AudioParameterFloat(					"mix",          		// parameterID
																	"Mix",          		// parameter name
																	0.0f,           		// minimum value
																	1.0f,           		// maximum value
																	0.5f ) );       		// default value
	
	addParameter( m_feedback = new AudioParameterFloat(				"feedback",     		// parameterID
																	"Feedback",     		// parameter name
																	0.0f,           		// minimum value
																	MAX_FEEDBACK,   		// maximum value
													   				0.5f ) );       		// default value
	
	addParameter( m_freeze_active = new AudioParameterBool(			"freeze",     			// parameterID
													   				"Freeze",     			// parameter name
													   				false ) );       		// default value
	
	addParameter( m_low_head_mix = new AudioParameterFloat(			"low_head_mix",    		// parameterID
																	"Mix",    				// parameter name
																	0.0f,           		// minimum value
																	1.0f,           		// maximum value
																	0.5f ) );       		// default value
	
	addParameter( m_low_head_size = new AudioParameterFloat(		"low_head_size",    	// parameterID
																   	"Size",    				// parameter name
																   	0.0f,           		// minimum value
																   	1.0f,           		// maximum value
																   	0.5f ) );       		// default value
	
	addParameter( m_low_head_jitter = new AudioParameterFloat(		"low_head_jitter",    	// parameterID
																	"Jitter",    			// parameter name
																	0.0f,           		// minimum value
																	1.0f,           		// maximum value
																	0.5f ) );       		// default value

	addParameter( m_normal_head_mix = new AudioParameterFloat(		"normal_head_mix",    	// parameterID
																	"Mix",    				// parameter name
																	0.0f,           		// minimum value
																	1.0f,           		// maximum value
																	0.5f ) );       		// default value
	
	addParameter( m_normal_head_size = new AudioParameterFloat(		"normal_head_size",    	// parameterID
																	"Size",  			  	// parameter name
																	0.0f,           		// minimum value
																	1.0f,           		// maximum value
																	0.5f ) );       		// default value
	
	addParameter( m_normal_head_jitter = new AudioParameterFloat(	"normal_head_jitter",	// parameterID
																	"Jitter",				// parameter name
																	0.0f,           		// minimum value
																	1.0f,           		// maximum value
																	0.5f ) );       		// default value
	
	addParameter( m_high_head_mix = new AudioParameterFloat(		"high_head_mix",    	// parameterID
																	"Mix",    				// parameter name
																	0.0f,           		// minimum value
																	1.0f,           		// maximum value
																	0.5f ) );       		// default value
	
	addParameter( m_high_head_size = new AudioParameterFloat(		"high_head_size",    	// parameterID
															   		"Size",    				// parameter name
															   		0.0f,           		// minimum value
																	1.0f,           		// maximum value
															   		0.5f ) );       		// default value
	
	addParameter( m_high_head_jitter = new AudioParameterFloat(		"high_head_jitter",		// parameterID
																	"Jitter",				// parameter name
																	0.0f,           		// minimum value
																	1.0f,           		// maximum value
																	0.5f ) );       		// default value
	
	addParameter( m_reverse_head_mix = new AudioParameterFloat(		"reverse_head_mix",		// parameterID
															   		"Mix",					// parameter name
															   		0.0f,           		// minimum value
															   		1.0f,           		// maximum value
															   		0.5f ) );       		// default value
}

GlitchDelayPluginAudioProcessor::~GlitchDelayPluginAudioProcessor()
{
}

//==============================================================================
const String GlitchDelayPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GlitchDelayPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GlitchDelayPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GlitchDelayPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GlitchDelayPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GlitchDelayPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GlitchDelayPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GlitchDelayPluginAudioProcessor::setCurrentProgram (int index)
{
}

const String GlitchDelayPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void GlitchDelayPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void GlitchDelayPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GlitchDelayPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GlitchDelayPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void GlitchDelayPluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals no_denormals;
	
	if( m_prev_buffer.getNumChannels() != 1 || m_prev_buffer.getNumSamples() != buffer.getNumSamples() )
	{
		// set size of prev_buffer if not correct dimensions
		m_prev_buffer.setSize( 1, buffer.getNumSamples() );
		m_prev_buffer.clear( 0, 0, m_prev_buffer.getNumSamples() );
	}
	else
	{
		// blend feedback into input
		mix_into( buffer, m_prev_buffer, 0, 1.0f - (*m_feedback), *m_feedback );
	}
	
	m_effect->pre_process_audio( buffer, m_effect->num_input_channels(), m_effect->num_output_channels() );

	m_effect->set_loop_size( LOW_HEAD, *m_low_head_size );
	m_effect->set_jitter( LOW_HEAD, *m_low_head_jitter );
	m_effect->set_loop_size( NORMAL_HEAD, *m_normal_head_size );
	m_effect->set_jitter( NORMAL_HEAD, *m_normal_head_jitter );
	m_effect->set_loop_size( HIGH_HEAD, *m_high_head_size );
	m_effect->set_jitter( HIGH_HEAD, *m_high_head_jitter );
	
	m_effect->set_freeze_active( *m_freeze_active );
	
	m_effect->set_loop_moving(false);
	m_effect->update();
	
	AudioSampleBuffer output( m_effect->num_output_channels(), buffer.getNumSamples() );
	m_effect->post_process_audio( output );
	
	// mix down effect output to 1 channel
	std::array<float, 4> mix_weights = { (*m_low_head_mix) * 0.25f, (*m_normal_head_mix) * 0.25f, (*m_high_head_mix) * 0.25f, (*m_reverse_head_mix) * 0.25f };
	mix_down( output, mix_weights );
	
	// mix output with original input
	mix_into( output, buffer, 0, *m_mix, 1.0f - *(m_mix) );
	
	// copy our mixed output to channel 0 of buffer
	buffer.copyFrom( 0, 0, output, 0, 0, output.getNumSamples() );
	
	// setup feedback for next block
	m_prev_buffer.copyFrom( 0, 0, output, 0, 0, output.getNumSamples() );
}

//==============================================================================
bool GlitchDelayPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GlitchDelayPluginAudioProcessor::createEditor()
{
    return new GlitchDelayPluginAudioProcessorEditor(*this, *m_effect);
}

//==============================================================================
void GlitchDelayPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GlitchDelayPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GlitchDelayPluginAudioProcessor();
}
