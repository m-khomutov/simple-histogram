//
// Created by mkh on 07.11.2023.
//

#ifndef AACHISTOGRAM_ADTS_H
#define AACHISTOGRAM_ADTS_H

#include <fstream>

class bitreader {
public:
    bitreader( uint8_t *data, size_t size )
    : m_data( data )
    , m_size( size )
    , m_pos( m_data )
    {}

    bool bit() {
        if( m_bits > 7 ) {
            if( ++m_pos == m_data + m_size )
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
    uint8_t *m_data;
    size_t m_size;
    uint8_t *m_pos;
    uint8_t m_bits = 0;
};

class adts {
public:
    enum MPEGVersion { MPEG4 = 0, MPEG2 };
    enum Profile { Main = 0, lc, ssr };

    adts( std::ifstream &f );

    size_t size() const
    {
        return m_protectionAbsence ? SIZE - 2 : SIZE;
    }
    uint8_t *raw()
    {
        return m_raw;
    }
    Profile profile() const
    {
        return Profile(m_profile);
    }
    size_t framesize() const
    {
        return m_frameLength;
    }
    uint8_t fsindex() const
    {
        return m_sampling_frequency_index;
    }
    uint32_t frequency() const
    {
        return Frequency[m_sampling_frequency_index];
    }
    uint32_t channels() const
    {
        return m_channel_configuration;
    }

private:
    static const uint16_t SYNCWORD = 0x0fff;

    const uint32_t Frequency[13] {
            96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350,
    };
    static const size_t SIZE = 9;

    uint8_t m_raw[ SIZE ];

    uint16_t m_syncword {0xfff};
    MPEGVersion m_MPEGversion {MPEG4};
    uint8_t m_layer {0};
    bool m_protectionAbsence {true};
    uint8_t m_profile;
    uint8_t m_sampling_frequency_index;
    bool m_private {false};
    uint8_t m_channel_configuration;
    bool m_original {true};
    bool m_home {false};
    bool m_copyrightID[2];
    uint16_t m_frameLength {0};
    uint16_t m_fullness;
    uint8_t m_frameCount;
    uint16_t m_crc;
};


#endif //AACHISTOGRAM_ADTS_H
