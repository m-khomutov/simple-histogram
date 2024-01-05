//
// Created by mkh on 07.11.2023.
//

#ifndef AACHISTOGRAM_AACREADER_H
#define AACHISTOGRAM_AACREADER_H

#include "aplayer.h"
#include "aacdec.h"

#include <fstream>
#include <memory>
#include <vector>

class aacreader {
public:
    aacreader( char const *filename );
    ~aacreader();

    bool ready() const;
    void readnext();
    uint64_t const * histogram() const
    {
        return m_decoder->histogram();
    }
    size_t const histogram_size() const
    {
        return m_decoder->histogram_size();
    }

private:
    std::ifstream m_ifile;
    std::shared_ptr< aplayer > m_aplayer_;
    std::vector< uint8_t > m_frame;
    std::shared_ptr< decoder > m_decoder;
};


#endif //AACHISTOGRAM_AACREADER_H
