//
// Created by mkh on 28.12.2023.
//

#include <iostream>
#include "decoder.h"

aac::bitreader::bitreader( uint8_t *data, size_t size )
: m_data( data, data + size )
, m_pos( m_data.data() )
{}


aac::frame::frame( uint8_t *data, size_t size )
: m_br( data, size )
{
    m_br.align();

    uint32_t id_syn_ele;
    while( (id_syn_ele = m_br.bits( 3 )) != ElementID::END ) { /* get audio syntactic element */
        std::cerr << "EL: " << id_syn_ele << std::endl;
        switch( id_syn_ele ) {
            case ElementID::SCE:
                f_single_channel_element();
                break;
            case ElementID::CPE:
                f_channel_pair_element();
                break;
            case ElementID::CCE:
                f_coupling_channel_element();
                break;
            case ElementID::FXE:
                f_lfe_channel_element();
                break;
            case ElementID::PCE:
                f_program_config_element();
                break;
            case ElementID::FLE:
                f_fill_element();
                break;
            case ElementID::DSE:
                f_data_stream_element();
                break;
            default:
                return;
        }
    }
}

void aac::frame::f_single_channel_element()
{
    m_element_identifier_tag = m_br.bits( 4 );
    f_individual_channel_stream( 0 );
}

void aac::frame::f_channel_pair_element()
{
    m_element_identifier_tag = m_br.bits( 4 );
    uint32_t common_window = m_br.bit();
    if( common_window ) {
        f_ics_info();
        m_ms_mask_present = m_br.bits( 2 );
        std::cerr << "mask present " <<m_ms_mask_present << std::endl;
        if( m_ms_mask_present == 1 ) {
            for( size_t sfb(0); sfb < m_max_sfb; ++sfb ) {
                m_ms_used[sfb] = m_br.bit();
            }
        }
    }
    f_individual_channel_stream( common_window );
    f_individual_channel_stream( common_window );
}

void aac::frame::f_coupling_channel_element()
{}

void aac::frame::f_lfe_channel_element()
{}

void aac::frame::f_program_config_element()
{}

void aac::frame::f_fill_element()
{
    uint32_t cnt = m_br.bits( 4 );
    if( cnt == 15 ) {
        uint32_t esc_l_cnt;
        do {
            esc_l_cnt = m_br.bits( 8 );
            cnt += esc_l_cnt - 1;
        }
        while( esc_l_cnt == 0xff );
    }
    for( size_t i(0); i < cnt; ++i )
        m_br.bits( 8 );
}

void aac::frame::f_data_stream_element()
{}

void aac::frame::f_ics_info()
{
    m_window_sequence = m_br.bits( 3 );
    uint32_t window_shape = m_br.bit();
    std::cerr << "WSEQ = " << m_window_sequence << " WSHAPE = " << window_shape << std::endl;

    if( m_window_sequence != WindowSequece::EIGHT_SHORT ) { // long window (1024)
        m_max_sfb = m_br.bits( 6 );
        std::cerr << " sbf_l: " << m_max_sfb << std::endl;
    }
    if( !(m_window_sequence == WindowSequece::ONLY_LONG || m_window_sequence == WindowSequece::LONG_START || m_window_sequence == WindowSequece::LONG_STOP) ) {
        // short windows (128)
        m_max_sfb = m_br.bits( 4 );
        m_scale_factor_grouping = m_br.bits( 7 );
    }
    else {
        uint32_t predictor_data_present = m_br.bit();
        std::cerr << "PRED: " << predictor_data_present << std::endl;
        if( predictor_data_present ) {
            uint32_t predictor_reset = m_br.bit();
            std::cerr << "PRED REST: " << predictor_reset << std::endl;
            if( predictor_reset ) {
                m_predictor_reset_index = m_br.bits( 5 );
            }
            for( size_t sfb(0); sfb < std::min( m_max_sfb, PRED_SFB_MAX ); ++sfb ) {
                m_prediction_used[sfb] = m_br.bit();
            }
        }
    }
}

void aac::frame::f_individual_channel_stream( uint32_t common_window )
{
    if(!common_window)
        f_ics_info();

    f_section_data();
    f_scale_factor_data();

    uint32_t pulse_data_present = m_br.bit();
    if( pulse_data_present ) {
        f_pulse_data();
    }

    uint32_t tns_data_present = m_br.bit();
    if( tns_data_present ) {
        f_tns_data();
    }

    uint32_t gain_control_data_present = m_br.bit();
    if( gain_control_data_present ) {
        f_gain_control_data();
    }

    f_spectral_data();
}

void aac::frame::f_section_data()
{
    m_num_sec = 0;
    uint32_t top = 49;
    if( m_window_sequence == WindowSequece::EIGHT_SHORT )
        top = 8 * 14;

    int k = 0;
    while ( k < top) {
        m_sect_cb[m_num_sec] = m_br.bits( 4 );
        uint32_t len = 0;
        uint32_t sec_len_incr;
        //while( (sec_len_incr = m_br.bits( 3 )) == MAX_INC_LEN )
        //    len += MAX_INC_LEN;
        len += sec_len_incr;
        //sect_start[m_num_sec]=k
        //sect_end[m_num_sec]=k+len
        k += len;
        ++m_num_sec;
    }
}

void aac::frame::f_scale_factor_data()
{}

void aac::frame::f_pulse_data()
{
    uint32_t number_pulse = m_br.bits( 2 );
    uint32_t pulse_start_sfb = m_br.bits( 6 );
    for( int i(0); i < number_pulse + 1; ++i ) {
        uint32_t pulse_offset = m_br.bits( 5 );
        uint32_t pulse_amp = m_br.bits( 4 );
    }
}

void aac::frame::f_tns_data()
{}

void aac::frame::f_gain_control_data()
{
    uint32_t max_band = m_br.bits( 2 );

    if( m_window_sequence == WindowSequece::ONLY_LONG ) {
        for( size_t bd(0); bd <= max_band; ++bd ) {
            for( size_t wd(0); wd < 1; ++wd ) {
                uint32_t adjust_num = m_br.bits( 3 );
                for( size_t ad(0); ad < adjust_num; ++ad ) {
                    uint32_t alevcode = m_br.bits( 4 );
                    uint32_t aloccode = m_br.bits( 5 );
                }
            }
        }
    }
    else if( m_window_sequence == WindowSequece::LONG_START ) {
        for( size_t bd(0); bd <= max_band; ++bd ) {
            for( size_t wd(0); wd < 2; ++wd ) {
                uint32_t adjust_num = m_br.bits( 3 );
                for( size_t ad(0); ad < adjust_num; ++ad ) {
                    uint32_t alevcode = m_br.bits( 4 );
                    if( wd == 0 ) {
                        uint32_t aloccode = m_br.bits( 4 );
                    }
                    else
                        uint32_t aloccode = m_br.bits( 2 );
                }
            }
        }
    }
    else if( m_window_sequence == WindowSequece::EIGHT_SHORT ) {
        for( size_t bd(0); bd <= max_band; ++bd ) {
            for( size_t wd(0); wd < 8; ++wd ) {
                uint32_t adjust_num = m_br.bits( 3 );
                for( size_t ad(0); ad < adjust_num; ++ad ) {
                    uint32_t alevcode = m_br.bits( 4 );
                    uint32_t aloccode = m_br.bits( 2 );
                }
            }
        }
    }
    else if( m_window_sequence == WindowSequece::LONG_STOP ) {
        for( size_t bd(0); bd <= max_band; ++bd ) {
            for( size_t wd(0); wd < 2; ++wd ) {
                uint32_t adjust_num = m_br.bits( 3 );
                for( size_t ad(0); ad < adjust_num; ++ad ) {
                    uint32_t alevcode = m_br.bits( 4 );
                    if( wd == 0 ) {
                        uint32_t aloccode = m_br.bits( 4 );
                    }
                    else
                        uint32_t aloccode = m_br.bits( 5 );
                }
            }
        }
    }
}

void aac::frame::f_spectral_data()
{
    for( int i(0); i < m_num_sec; ++i ) {

    }
}
