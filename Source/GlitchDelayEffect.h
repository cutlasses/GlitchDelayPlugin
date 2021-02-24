#pragma once

#include "Util.h"


static const int DELAY_BUFFER_SIZE_IN_BYTES(1024*240);      // 240k

////////////////////////////////////

class DELAY_BUFFER;
class GLITCH_DELAY_INTERFACE;

////////////////////////////////////

class PLAY_HEAD
{
	const DELAY_BUFFER&         m_delay_buffer;     // TODO pass in to save storage?
    const int                   m_fade_samples;
    
	float                       m_current_play_head;
	float                       m_destination_play_head;
	float                       m_play_speed;       // negative means play in reverse (loop sections currently not supported)
	int                         m_fade_samples_remaining;
	
	int                         m_loop_start;
	int                         m_loop_end;
	int                         m_unjittered_loop_start;
	int                         m_shift_speed;
	
	float                       m_next_loop_size_ratio;
	float                       m_next_shift_speed_ratio;
	float                       m_jitter_ratio;
	
	bool                        m_initial_loop_crossfade_complete;
	
	int                         play_head_to_write_head_buffer_size() const;
	int16_t                     read_sample_with_cross_fade();
	
public:
	
	PLAY_HEAD( const DELAY_BUFFER& delay_buffer, float play_speed, int fade_samples );
	
	int                         current_position() const;
	int                         destination_position() const;
	
	int                         loop_start() const;
	int                         loop_end() const;
	int                         buffered_loop_start() const;
	int                         current_loop_size() const;
	
	bool                        looping() const;
    void                        check_write_head_collision(int write_position);
	bool                        position_inside_section( int position, int start, int end ) const;
	bool                        position_inside_next_read( int position, int read_size ) const;
	bool                        crossfade_active() const;
	bool                        initial_loop_crossfade_complete() const;
	bool                        play_forwards() const;
	
	void                        set_loop_size( float loop_size_ratio );
	void                        set_shift_speed( float speed );
	void                        set_jitter( float jitter );
	void                        set_play_head( int offset_from_write_head );
	void                        set_next_loop();
	
	void                        set_behind_write_head();
	
	void                        read_from_play_head( int16_t* dest, int size );
	
	void                        enable_loop( int start, int end );
	void                        disable_loop();
	
#ifdef DEBUG_OUTPUT
	void                        debug_output();
#endif
};

////////////////////////////////////

class DELAY_BUFFER
{
	friend PLAY_HEAD;
	
	uint8_t                     m_buffer[DELAY_BUFFER_SIZE_IN_BYTES];
	int                         m_buffer_size_in_samples;
	int                         m_sample_size_in_bits;
	
	int                         m_write_head;
	
	int                         m_fade_samples_remaining;
	
	bool						m_freeze_active;
	
	/////////
	void                        fade_in_write();

	
public:
	
	DELAY_BUFFER();
	
	int                         position_offset_from_head( int offset ) const;
	int                         delay_offset_from_ratio( float ratio ) const;
	int                         delay_offset_from_time( int time_in_ms ) const;
	int                         write_head() const;
	int                         wrap_to_buffer( int position ) const;
	bool                        write_buffer_fading_in() const;
	
	void                        write_sample( int16_t sample, int index );
	int16_t                     read_sample( int index ) const;
	int16_t                     read_sample_with_speed( float index, float speed ) const;
	
	void                        increment_head( int& head ) const;
	void                        increment_head( float& head, float speed ) const;
	
	void                        write_to_buffer( const int16_t* source, int size );
	
	void                        set_bit_depth( int sample_size_in_bits );
	
	bool						freeze_active() const;
	void						set_freeze( bool freeze );
	
#ifdef DEBUG_OUTPUT
	void                        debug_output();
#endif
};

////////////////////////////////////

class GLITCH_DELAY_EFFECT
{	
public:

  static constexpr int     NUM_PLAY_HEADS = 4;
  
private:
  
    const GLITCH_DELAY_INTERFACE&   m_interface;
    
	DELAY_BUFFER          	m_delay_buffer;
	
	PLAY_HEAD             	m_play_heads[NUM_PLAY_HEADS];
		
public:
	
	GLITCH_DELAY_EFFECT(const GLITCH_DELAY_INTERFACE& interface);
		
	void                    update( const int16_t* input_sample_data, int num_samples ); // read from sample data, then output to it
    void                    fill_output( int16_t* output_sample_data, int num_samples, int channel );

	// for plugin display only
	int                   	num_heads() const;
	void                  	head_ratio_details( int head, float& loop_start, float& loop_end, float& current_position ) const;
};
