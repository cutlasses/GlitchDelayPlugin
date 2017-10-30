
#include "GlitchDelayEffect.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================

namespace
{
	void print_rect( const Rectangle<int> rect, const char* name )
	{
		std::cout << name << " (" << rect.getTopLeft().x << "," << rect.getTopLeft().y << ") width:" << rect.getWidth() << " height:" << rect.getHeight() << "\n";
	}
}

GLITCH_DELAY_VIEW::GLITCH_DELAY_VIEW( int num_heads ) :
    m_tl_x(0),
    m_tl_y(0),
    m_width(0),
    m_height(0)
{
    m_heads.resize( num_heads, DELAY_HEAD_PROXY() );
    m_heads.back().m_write_head = true;
}

void GLITCH_DELAY_VIEW::set_dimensions( int x, int y, int width, int height )
{
    m_tl_x      = x;
    m_tl_y      = y;
    m_width     = width;
    m_height    = height;
}

void GLITCH_DELAY_VIEW::update( const GLITCH_DELAY_EFFECT& effect )
{
    for( int h = 0; h < m_heads.size(); ++h )
    {
        DELAY_HEAD_PROXY& head  = m_heads[h];
        
        effect.head_ratio_details( h, head.m_start, head.m_end, head.m_current_position );
    }
}

void GLITCH_DELAY_VIEW::paint( Graphics& g )
{
    g.setColour( Colours::aquamarine );
    g.drawRect( m_tl_x, m_tl_y, m_width, m_height );
    
    g.setColour( Colours::white );
    
    for( const DELAY_HEAD_PROXY& head : m_heads )
    {
        Point<int> tl, br;
        if( head.m_start == head.m_end )
        {
            // write head (or head without loop) - fills the entire area in height
            if( head.m_write_head )
            {
                g.setColour( Colours::goldenrod );
            }
            tl = Point<int>( m_tl_x + roundToInt( head.m_current_position * m_width ), m_tl_y );
            br = Point<int>( tl.getX() + 5, tl.getY() + m_height );
        }
        else
        {
            // draw the current position
            const Point<int>cp_tl( m_tl_x + roundToInt( head.m_current_position * m_width ), m_tl_y );
            const Point<int>cp_br ( cp_tl.getX() + 1, cp_tl.getY() + m_height );
            const Rectangle<int> cp_rect( cp_tl, cp_br );
            g.fillRect( cp_rect );
            
            g.setColour( Colours::lightgrey );
            const float height_ratio    = 0.65f;
            const int loop_height       = roundToInt( m_height * height_ratio );
            const int height_offset     = ( m_height - loop_height ) / 2;
            tl = Point<int>( m_tl_x + roundToInt( head.m_start * m_width ), m_tl_y + height_offset );
            br = Point<int>( m_tl_x + roundToInt( head.m_end * m_width ), tl.getY() + loop_height );
        }
        
        if( tl.x < br.x )
        {
            const Rectangle<int> head_rect( tl, br );
            
            g.fillRect( head_rect );
        }
        else
        {
            // region crosses the end of the buffer - draw 2 rects
            const Point<int> new_br( m_tl_x + m_width, br.y );
            const Rectangle<int> head_rect1( tl, new_br );
            
            g.fillRect( head_rect1 );
            
            const Point<int> new_tl( m_tl_x, tl.y );
            const Rectangle<int> head_rect2( new_tl, br );
            
            g.fillRect( head_rect2 );
        }
    }
}

///////////////////////////////////////////////////////////////////////////

const int GlitchDelayPluginAudioProcessorEditor::HEAD_DIAL_ROW_COUNT_MAX	= 3;
const int GlitchDelayPluginAudioProcessorEditor::DIAL_SEPARATION       		= 30;
const int GlitchDelayPluginAudioProcessorEditor::DIAL_SIZE_PRIMARY     		= 95;
const int GlitchDelayPluginAudioProcessorEditor::DIAL_SIZE_SECONDARY   		= 75;
const int GlitchDelayPluginAudioProcessorEditor::BUTTON_SIZE				= 70;
const int GlitchDelayPluginAudioProcessorEditor::HEAD_LABEL_HEIGHT			= 14;
const int GlitchDelayPluginAudioProcessorEditor::DIAL_LABEL_HEIGHT         	= 10;
const int GlitchDelayPluginAudioProcessorEditor::GLITCH_DELAY_HEIGHT   		= 60;
const int GlitchDelayPluginAudioProcessorEditor::BORDER                		= 40;

const char* head_names[] = { "Low", "Normal", "High", "Reverse" };

GlitchDelayPluginAudioProcessorEditor::GlitchDelayPluginAudioProcessorEditor (GlitchDelayPluginAudioProcessor& p, const GLITCH_DELAY_EFFECT& effect) :
    AudioProcessorEditor(&p),
    Slider::Listener(),
	ToggleButton::Listener(),
    Timer(),
    m_processor(p),
    m_effect(effect),
	m_all_dials(),
	m_all_labels(),
    m_main_dials(),
    m_main_dial_labels(),
	m_head_descr_labels(),
	m_head_dials(),
	m_head_dial_labels(),
	m_freeze_button(nullptr),
	m_all_float_parameters(),
	m_all_bool_parameters(),
    m_num_head_dial_rows(0),
	m_max_head_label_width(0),
	m_primary_row_width(0),
	m_secondary_row_width(0),
    m_glitch_view(nullptr)
{
    const OwnedArray<AudioProcessorParameter>& params = p.getParameters();
    
    for( int i = 0; i < params.size(); ++i )
    {
        if( AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*>(params[i]) )
        {
            Slider* slider = new Slider( param->name );
			slider->setRange( param->range.start, param->range.end );
			slider->setSliderStyle( Slider::RotaryHorizontalDrag );
			slider->setValue( *param );
			slider->setTextBoxStyle( Slider::NoTextBox, false, 0, 0 );
			
			slider->addListener( this );
			addAndMakeVisible(slider );
			
			Label* label = new Label( param->name, param->name );
			label->setJustificationType( Justification::centred );
			label->setFont( Font(DIAL_LABEL_HEIGHT) );
			addAndMakeVisible( label );
			
			if( param->paramID.contains( "head" ) )
			{
				m_head_dials.push_back( slider );
				m_head_dial_labels.push_back( label );
			}
			else
			{
				m_main_dials.push_back( slider );
				m_main_dial_labels.push_back( label );
			}
			
			m_all_dials.add( slider );
			m_all_labels.add( label );
			
			m_all_float_parameters.push_back( param );
        }
		else if( AudioParameterBool* param = dynamic_cast<AudioParameterBool*>(params[i]) )
		{
			ASSERT_MSG( m_freeze_button == nullptr, "More than one bool parameter?" );
			
			m_freeze_button = new ToggleButton( param->name );
			m_freeze_button->addListener( this );
			addAndMakeVisible( m_freeze_button );
			//m_freeze_button->changeWidthToFitText();
			
			m_all_bool_parameters.push_back( param );
		}
    }
	
	for( int n = 0; n < effect.num_heads() - 1; ++n )
	{
		const char* head_name	= head_names[n];
		Label* descr_label		= new Label( head_name, head_name );
		descr_label->setJustificationType( Justification::right );
		descr_label->setFont( Font(HEAD_LABEL_HEIGHT) );
		addAndMakeVisible( descr_label );
		
		int border				= descr_label->getBorderSize().getLeftAndRight();
		m_max_head_label_width	= max_val( m_max_head_label_width, descr_label->getFont().getStringWidth( head_name ) + border );
		
		m_head_descr_labels.add( descr_label );
	}
	
    m_glitch_view           	= make_unique<GLITCH_DELAY_VIEW>( effect.num_heads() );
    
    m_num_head_dial_rows    	= static_cast<int>(m_head_dials.size()) / HEAD_DIAL_ROW_COUNT_MAX;
    if( m_head_dials.size() % HEAD_DIAL_ROW_COUNT_MAX != 0 )
    {
        ++m_num_head_dial_rows;
    }
	
	m_primary_row_width 		= ( static_cast<int>(m_main_dials.size()) * DIAL_SIZE_PRIMARY ) + BUTTON_SIZE/*m_freeze_button->getWidth()*/;
	m_secondary_row_width		= m_max_head_label_width + ( DIAL_SIZE_PRIMARY * HEAD_DIAL_ROW_COUNT_MAX );
    const int width      		= ( BORDER * 2.0f ) + max_val( m_primary_row_width, m_secondary_row_width );
    const int height      		= ( BORDER * 2.0f ) + DIAL_SIZE_PRIMARY + ( DIAL_SIZE_SECONDARY * ( m_num_head_dial_rows ) ) + (DIAL_SEPARATION * ( m_num_head_dial_rows + 1 )) + GLITCH_DELAY_HEIGHT;
    setSize( width, height );
    
    // start the callback timer
    startTimer (100);
    
}

GlitchDelayPluginAudioProcessorEditor::~GlitchDelayPluginAudioProcessorEditor()
{
}

AudioParameterFloat* GlitchDelayPluginAudioProcessorEditor::get_parameter_for_slider( Slider* slider )
{
    return m_all_float_parameters[ m_all_dials.indexOf(slider) ];
}

//==============================================================================
void GlitchDelayPluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    g.setColour (Colours::white);
    g.setFont (15.0f);
    
    m_glitch_view->paint(g);
}

void GlitchDelayPluginAudioProcessorEditor::resized()
{
	Rectangle<int> reduced = getLocalBounds().reduced( BORDER );
    
	auto add_dial = []( Rectangle<int>& row_rect, Slider& dial, int dial_size, Label& label, int label_size )
	{
		Rectangle<int> dial_bounds          = row_rect.removeFromLeft( dial_size );
		const Rectangle<int> label_bounds   = dial_bounds.removeFromBottom( label_size );
		
		dial.setBounds( dial_bounds );
		label.setBounds( label_bounds );
	};
	
	// main row
	Rectangle<int> main_row_rect		= reduced.removeFromTop( DIAL_SIZE_PRIMARY );
	int unused_width					= main_row_rect.getWidth() - m_primary_row_width;
	main_row_rect.reduce( unused_width / 2, 0 );
	
	
	for( int col = 0; col < m_main_dials.size(); ++col  )
	{
		add_dial( main_row_rect, *m_main_dials[col], DIAL_SIZE_PRIMARY, *m_main_dial_labels[col], DIAL_LABEL_HEIGHT );
	}
	//ASSERT_MSG( m_freeze_button->getWidth() == main_row_rect.getWidth(), "Invalid row width remaining" );
	m_freeze_button->setBounds( main_row_rect );
	// leave space between each row
	reduced.removeFromTop( DIAL_SEPARATION );
	
	// head rows
	int dial( 0 );
	for( int row = 0; row < m_num_head_dial_rows; ++row )
	{
		Rectangle<int> row_rect			= reduced.removeFromTop( DIAL_SIZE_SECONDARY );
		
		Rectangle<int> head_label_rect	= row_rect.removeFromLeft( m_max_head_label_width );
		m_head_descr_labels[row]->setBounds( head_label_rect );
		
		const int row_size				= min_val<int>( HEAD_DIAL_ROW_COUNT_MAX, static_cast<int>(m_head_dials.size()) - dial );
		
		for( int col = 0; col < row_size; ++col )
		{
			add_dial( row_rect, *m_head_dials[dial], DIAL_SIZE_SECONDARY, *m_head_dial_labels[dial], DIAL_LABEL_HEIGHT );
			++dial;
		}
		
		// leave space between each row
		reduced.removeFromTop( DIAL_SEPARATION );
	}
    
    Rectangle<int> glitch_rect = reduced.removeFromTop( GLITCH_DELAY_HEIGHT );
    Point<int> glitch_top_left = glitch_rect.getTopLeft();
    
    m_glitch_view->set_dimensions( glitch_top_left.getX(), glitch_top_left.getY(), glitch_rect.getWidth(), glitch_rect.getHeight() );
}

void GlitchDelayPluginAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if( AudioParameterFloat* param = get_parameter_for_slider(slider) )
    {
        *param = slider->getValue();
    }
}

void GlitchDelayPluginAudioProcessorEditor::sliderDragStarted (Slider* slider)
{
    if( AudioParameterFloat* param = get_parameter_for_slider( slider ) )
    {
        param->beginChangeGesture();
    }
}

void GlitchDelayPluginAudioProcessorEditor::sliderDragEnded (Slider* slider)
{
    if( AudioParameterFloat* param = get_parameter_for_slider( slider ) )
    {
        param->endChangeGesture();
    }
}

void GlitchDelayPluginAudioProcessorEditor::buttonClicked( Button* button )
{
	// currently only one button
	ASSERT_MSG( button == m_freeze_button, "What button is this?" );
	
	AudioParameterBool* param = m_all_bool_parameters.front();
	
	*param = !param->get();
}

void GlitchDelayPluginAudioProcessorEditor::timerCallback()
{
	/*
    const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
    
    for( int i = 0; i < params.size(); ++i )
    {
        if( const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]) )
        {
            if( i < m_all_dials.size() )
            {
                m_all_dials[i]->setValue(*param);
            }
        }
    }
	 */
    
    m_glitch_view->update( m_effect );
	
	// TODO only repaint the glitch vis (make it a component)
    repaint();
}
