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
        if( !m_aacdec_ ) {
            m_aacdec_.reset( new aacdec );
            m_aplayer_.reset( new aplayer( adts_.frequency(), adts_.channels() ));
        }
        if( m_frame.size() < adts_.framesize() ) {
            m_frame.resize( adts_.framesize() );
        }
        memcpy( m_frame.data(), adts_.raw(), adts_.size() );
        m_ifile.read( (char*)(m_frame.data() + adts_.size()), adts_.framesize() - adts_.size() );
        m_aacdec_->decode( m_frame.data(), adts_.framesize(), m_aplayer_.get() );
    }
    catch ( ... ) {
        m_ifile.close();
        throw;
    }
}
