//
// Created by mkh on 07.11.2023.
//

#include <iostream>
#include "aacdec.h"

#include <cstdio>
#include <cmath>
#include <cstring>

decoder::decoder( uint8_t *raw, size_t size )
: m_handle( NeAACDecOpen() )
, m_conf( NeAACDecGetCurrentConfiguration( m_handle ) )
{
    NeAACDecSetConfiguration( m_handle, m_conf );
    unsigned long samplerate;
    unsigned char channels;
    char err = NeAACDecInit( m_handle, raw, size, &samplerate, &channels );
    if (err != 0) {
        // Handle error
        fprintf(stderr, "NeAACDecInit error: %d\n", err);
    }
    fprintf(stderr, "{ samplerate: %lu, channels: %u, bytesRead: %d }\n", samplerate, channels, err);
}

decoder::~decoder()
{
    NeAACDecClose( m_handle );
}

int16_t * decoder::decode( uint8_t *data, size_t size )
{
    void *output = NeAACDecDecode( m_handle, &m_info, data, size );
    if ((m_info.error == 0) && (m_info.samples > 0)) {
        // do what you need to do with the decoded samples
        fprintf(stderr, "decoded %lu samples\n", m_info.samples);
        fprintf(stderr, "  bytesconsumed: %lu\n", m_info.bytesconsumed);
        fprintf(stderr, "  channels: %d\n", m_info.channels);
        fprintf(stderr, "  samplerate: %lu\n", m_info.samplerate);
        fprintf(stderr, "  sbr: %u\n", m_info.sbr);
        fprintf(stderr, "  object_type: %u\n", m_info.object_type);
        fprintf(stderr, "  header_type: %u\n", m_info.header_type);
        fprintf(stderr, "  num_front_channels: %u\n", m_info.num_front_channels);
        fprintf(stderr, "  num_side_channels: %u\n", m_info.num_side_channels);
        fprintf(stderr, "  num_back_channels: %u\n", m_info.num_back_channels);
        fprintf(stderr, "  num_lfe_channels: %u\n", m_info.num_lfe_channels);
        fprintf(stderr, "  ps: %u\n", m_info.ps);
        fprintf(stderr, "\n");

        memset( m_histogram, 0, sizeof(m_histogram) );
        short *ptr = (short*)output;
        for( size_t i(0); i < m_info.samples; i += 2 ) {
            float db = std::log2f(fabsf( float(*ptr) ));
            if( !std::isinf( db ) ) {
                m_histogram[uint8_t(db * DBperBit) % SIZE]++;
            }
            ptr++;
        }
        return (int16_t*)output;
    } else if (m_info.error != 0) {
        // Some error occurred while decoding this frame
        fprintf(stderr, "NeAACDecode error: %d\n", m_info.error);
        fprintf(stderr, "%s\n", NeAACDecGetErrorMessage(m_info.error));
    } else {
        fprintf(stderr, "got nothing...\n");
    }
    return nullptr;
}

size_t const decoder::histogram_size() const
{
    return SIZE;
}

/*#include "aplayer.h"

float clip( float sample )
{
    return sample < -1.f ? -1.f : (sample > 1.f ? 1.f : sample);
}

aacdec::aacdec()
{
    avcodec_register_all();

    m_codec = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (!m_codec) {
        throw std::logic_error( "Codec AAC not found" );
    }

    m_context = avcodec_alloc_context3( m_codec );
    if( !m_context ) {
        throw std::logic_error( "Could not allocate audio codec context" );
    }

    if( avcodec_open2( m_context, m_codec, nullptr) < 0 ) {
        av_free( m_context );
        throw std::logic_error( "Could not open codec");
    }

    if( !(m_frame = av_frame_alloc()) )
    {
        avcodec_close( m_context );
        av_free( m_context );
        throw std::logic_error( "Could not allocate audio frame" );
    }

    av_init_packet( &m_avpkt );
}

aacdec::~aacdec()
{
    avcodec_flush_buffers( m_context );
    av_frame_free( &m_frame );
    avcodec_close( m_context );
    av_free( m_context );
}

void aacdec::decode( uint8_t *buf, size_t size, aplayer *player )
{
    ::memset( m_histogram, 0, sizeof(m_histogram) );

    m_avpkt.data = buf;
    m_avpkt.size = size;
    while( m_avpkt.size > 0 )
    {
        int got_frame = 0;
        int len = avcodec_decode_audio4( m_context, m_frame, &got_frame, &m_avpkt );
        if (len < 0) {
            std::cerr << "Error while decoding\n";
            return;
        }
        if( got_frame )
        {
            int planes = 0;
            for( ; planes < AV_NUM_DATA_POINTERS; ++planes ) {
                if( ! m_frame->data[planes] )
                    break;
            }
            if( m_samples.size() < m_frame->nb_samples * planes )
                m_samples.resize( m_frame->nb_samples * planes );

            float *out = m_samples.data();
            for( size_t i(0); i < m_frame->nb_samples; ++i )
            {
                float mean = 0.f;
                for( size_t plane(0); plane < planes; ++plane ) {
                    float * src = (float*)m_frame->data[plane];
                    mean += src[i];
                    *out ++ = src[i];
                }
                float db = 20 * log10( fabs(clip(mean / planes)) / MAX_LEVEL_VALUE );
                if( !std::isinf( db ) && db >= MIN_LEVEL_DB ) {
                    m_histogram[uint8_t(-1 * db)]++;
                }
            }
            player->play( (uint8_t*)m_samples.data(), m_frame->nb_samples );
        }
        m_avpkt.size -= len;
        m_avpkt.data += len;
        m_avpkt.dts = m_avpkt.pts = AV_NOPTS_VALUE;
    }
}
*/
