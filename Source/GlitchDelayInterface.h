/*
  ==============================================================================

    GlitchDelayInterface.h
    Created: 24 Feb 2021 7:06:59pm
    Author:  Scott Pitkethly

  ==============================================================================
*/

#pragma once

#include <array>

#include "GlitchDelayEffect.h"

class GLITCH_DELAY_INTERFACE
{
    // store 'next' values, otherwise interrupt could be called during calculation of values
    int                                 m_sample_size_in_bits               = 12;
    bool                                m_loop_moving                       = false;
    bool                                m_beat                              = false;
    bool                                m_freeze_active                     = false;

    std::array<float, GLITCH_DELAY_EFFECT::NUM_PLAY_HEADS>    m_loop_size_ratio;
    std::array<float, GLITCH_DELAY_EFFECT::NUM_PLAY_HEADS>    m_jitter_ratio;

    
public:
    
    GLITCH_DELAY_INTERFACE();
    
    int                         sample_size_in_bits() const;
    bool                        loop_moving() const;
    bool                        beat() const;
    bool                        freeze_active() const;
    
    float                       loop_size( int play_head ) const;
    float                       jitter( int play_head ) const;
        
    // in plugin these are set by the plugin editor (in hardware set inside this object)
    void                        set_sample_size_in_bits(int sample_size_in_bits);
    void                        set_loop_moving(bool moving);
    void                        set_beat(bool beat);
    void                        set_freeze_active(bool active);

    void                        set_loop_size( int play_head, float loop_size );
    void                        set_jitter( int play_head, float jitter );
};
