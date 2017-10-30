/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/


// forward declarations
class GLITCH_DELAY_EFFECT;

class GLITCH_DELAY_VIEW
{
    struct DELAY_HEAD_PROXY
    {
        DELAY_HEAD_PROXY() :
        m_start( 0.0f ),
        m_end( 0.0f ),
        m_current_position( 0.0f ),
        m_write_head(false)
        {
        }
        
        float                           m_start;
        float                           m_end;
        float                           m_current_position;
        bool                            m_write_head;
    };
    
    std::vector< DELAY_HEAD_PROXY >     m_heads;
    
    int                                 m_tl_x;         // top left x position
    int                                 m_tl_y;
    int                                 m_width;
    int                                 m_height;
    
public:
    
    GLITCH_DELAY_VIEW( int num_heads );
    
    void                                set_dimensions( int x, int y, int width, int height );
    
    void                                update( const GLITCH_DELAY_EFFECT& );
    void                                paint( Graphics& g );
};

///////////////////////////////////////////////////////////////////////////

class GlitchDelayPluginAudioProcessorEditor  :  public AudioProcessorEditor,
                                                public Slider::Listener,
												public ToggleButton::Listener,
                                                private Timer
{
public:
    GlitchDelayPluginAudioProcessorEditor(GlitchDelayPluginAudioProcessor&, const GLITCH_DELAY_EFFECT&);
    ~GlitchDelayPluginAudioProcessorEditor();

	AudioParameterFloat*                            get_parameter_for_slider( Slider* slider );

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //// Slider::Listener ////
    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    void sliderDragEnded (Slider* slider) override;
    /////////////////////////
	
	//// ToggleButton::Listener ////
	void buttonClicked (Button*) override;
	/////////////////////////

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    static const int                                HEAD_DIAL_ROW_COUNT_MAX;
    static const int                                DIAL_SIZE_PRIMARY;
	static const int                                DIAL_SIZE_SECONDARY;
	static const int								BUTTON_SIZE;
    static const int                                DIAL_SEPARATION;
	static const int                                HEAD_LABEL_HEIGHT;
    static const int                                DIAL_LABEL_HEIGHT;
    static const int                                GLITCH_DELAY_HEIGHT;
    static const int                                BORDER;

    GlitchDelayPluginAudioProcessor&                m_processor;
    
    const GLITCH_DELAY_EFFECT&                      m_effect;
	
	OwnedArray<Slider>                              m_all_dials;		// top row dials
	OwnedArray<Label>                               m_all_labels;
	
	std::vector<Slider*>                            m_main_dials;		// top row dials
    std::vector<Label*>                             m_main_dial_labels;
	
	OwnedArray<Label>								m_head_descr_labels;
	std::vector<Slider*>                            m_head_dials;		// dials for the tape heads
	std::vector<Label*>                             m_head_dial_labels;
	
	ScopedPointer<ToggleButton>						m_freeze_button;
	
	std::vector<AudioParameterFloat*>				m_all_float_parameters;
	std::vector<AudioParameterBool*>				m_all_bool_parameters;
	
    int                                             m_num_head_dial_rows;
	int												m_max_head_label_width;
	int												m_primary_row_width;
	int												m_secondary_row_width;
    
    std::unique_ptr<GLITCH_DELAY_VIEW>              m_glitch_view;
    
    
    void                                            timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlitchDelayPluginAudioProcessorEditor)
};
