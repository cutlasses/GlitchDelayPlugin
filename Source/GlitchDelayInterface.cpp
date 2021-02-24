/*
  ==============================================================================

    GlitchDelayInterface.cpp
    Created: 24 Feb 2021 7:06:14pm
    Author:  Scott Pitkethly

  ==============================================================================
*/

#include "GlitchDelayInterface.h"

GLITCH_DELAY_INTERFACE::GLITCH_DELAY_INTERFACE()
{
    m_loop_size_ratio.fill(0);
    m_jitter_ratio.fill(0);
}

int GLITCH_DELAY_INTERFACE::sample_size_in_bits() const
{
    return m_sample_size_in_bits;
}

bool GLITCH_DELAY_INTERFACE::loop_moving() const
{
    return m_loop_moving;
}

bool GLITCH_DELAY_INTERFACE::beat() const
{
    return m_beat;
}

bool GLITCH_DELAY_INTERFACE::freeze_active() const
{
    return m_freeze_active;
}

float GLITCH_DELAY_INTERFACE::loop_size( int play_head ) const
{
    return m_loop_size_ratio[play_head];
}

float GLITCH_DELAY_INTERFACE::jitter( int play_head ) const
{
    return m_jitter_ratio[play_head];
}

void GLITCH_DELAY_INTERFACE::set_sample_size_in_bits(int sample_size_in_bits)
{
    m_sample_size_in_bits = sample_size_in_bits;
}

void GLITCH_DELAY_INTERFACE::set_loop_moving(bool moving)
{
    m_loop_moving = moving;
}

void GLITCH_DELAY_INTERFACE::set_beat(bool beat)
{
    m_beat = beat;
}

void GLITCH_DELAY_INTERFACE::set_freeze_active(bool active)
{
    m_freeze_active = active;
}

void GLITCH_DELAY_INTERFACE::set_loop_size( int play_head, float loop_size )
{
    ASSERT_MSG( play_head < NUM_PLAY_HEADS, "Invalid play head index" );
    m_loop_size_ratio[play_head] = loop_size;
}

void GLITCH_DELAY_INTERFACE::set_jitter( int play_head, float jitter )
{
    ASSERT_MSG( play_head < NUM_PLAY_HEADS, "Invalid play head index" );
    m_jitter_ratio[play_head] = jitter;
}
