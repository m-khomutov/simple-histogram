//
// Created by mkh on 07.11.2023.
//

#include <iostream>
#include "aacdec.h"
#include "aplayer.h"

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
