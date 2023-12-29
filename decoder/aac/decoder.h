//
// Created by mkh on 28.12.2023.
//

#ifndef SIMPLE_HISTOGRAM_DECODER_H
#define SIMPLE_HISTOGRAM_DECODER_H

#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cmath>

namespace aac {

    class bitreader {
    public:
        bitreader( uint8_t *data, size_t size );

        bool bit() {
            if( m_bits > 7 ) {
                if( ++m_pos == m_data.data() + m_data.size() )
                    throw std::out_of_range( "[bitreader] Data finished" );
                m_bits = 0;
            }
            ++ m_bits;
            return (*m_pos >> (8 - m_bits)) & 1;
        }

        uint32_t bits( size_t n ) {
            uint32_t rc = 0;
            while( n ) {
                rc |= (bit() << (--n));
            }
            return rc;
        }

        bool bytealigned() const {
            return m_bits == 0 || m_bits > 7;
        }

        void align() {
            while( !bytealigned() )
                bit();
        }

        uint32_t nextbits( size_t n ) {
            uint8_t *p = m_pos;
            uint8_t b = m_bits;
            uint32_t rc = bits( n );

            m_bits = b;
            m_pos = p;
            return rc;
        }

    private:
        std::vector< uint8_t > m_data;
        uint8_t *m_pos;
        uint8_t m_bits = 0;
    };

    class frame {
    public:
        enum ElementID { SCE = 0, CPE, CCE, FXE, DSE, PCE, FLE, END };
        enum WindowSequece { ONLY_LONG = 0, LONG_START, EIGHT_SHORT, LONG_STOP };

        frame( uint8_t *data, size_t size );

    private:
        static constexpr uint32_t PRED_SFB_MAX = 41;

        bitreader m_br;
        uint32_t m_element_identifier_tag;
        uint32_t m_ms_mask_present;
        uint32_t m_max_sfb = 0;
        uint32_t m_ms_used[ 1<<6 ];
        uint32_t m_scale_factor_grouping;
        uint32_t m_prediction_used[PRED_SFB_MAX];
        uint32_t m_predictor_reset_index;
        uint32_t m_sect_cb[8 * 14];
        uint32_t m_window_sequence = 0;
        uint32_t m_num_sec;

    private:
        void f_single_channel_element();
        void f_channel_pair_element();
        void f_coupling_channel_element();
        void f_lfe_channel_element();
        void f_program_config_element();
        void f_fill_element();
        void f_data_stream_element();

        void f_ics_info();
        void f_individual_channel_stream( uint32_t common_window );
        void f_section_data();
        void f_scale_factor_data();
        void f_pulse_data();
        void f_tns_data();
        void f_gain_control_data();
        void f_spectral_data();
    };

    inline bool bit_set( uint32_t bit_field, uint32_t bit_num ) {
        return ( bit_field >> (int(log2(bit_field)) - bit_num) ) & 1;
    }
}  // namespace aac


#endif //SIMPLE_HISTOGRAM_DECODER_H
