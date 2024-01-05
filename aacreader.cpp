//
// Created by mkh on 07.11.2023.
//

#include "aacreader.h"
#include "adts.h"

aacreader::aacreader( const char *filename )
: m_ifile( filename, std::ios_base::in | std::ios_base::binary )
{
    if( !m_ifile.is_open() )
        throw std::logic_error( std::string( "failed to open " ) + filename );

    adts adts_( m_ifile );
    m_decoder.reset( new decoder( adts_.raw(), adts_.size() ) );
    m_aplayer_.reset( new aplayer( adts_.frequency(), adts_.channels() ) );
    m_ifile.seekg( 0 );
}

aacreader::~aacreader()
{
    m_ifile.close();
}

bool aacreader::ready() const
{
    return m_ifile.is_open() && !m_ifile.eof();
}

void aacreader::readnext()
{
    if( m_ifile.eof() )
        throw std::logic_error( "EOF" );

    try {
        adts adts_( m_ifile );
        if( m_frame.size() < adts_.framesize() ) {
            m_frame.resize( adts_.framesize() );
        }
        memcpy( m_frame.data(), adts_.raw(), adts_.size() );
        m_ifile.read( (char*)(m_frame.data() + adts_.size()), adts_.framesize() - adts_.size() );
        int16_t *out = m_decoder->decode( m_frame.data(), adts_.framesize() );
        if( out ) {
            m_aplayer_->play( (uint8_t*)out, m_decoder->samples() / m_decoder->channels() );
        }
    }
    catch ( ... ) {
        m_ifile.close();
        throw;
    }
}
