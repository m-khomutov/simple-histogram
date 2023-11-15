//
// Created by mkh on 07.11.2023.
//

#ifndef AACHISTOGRAM_AACREADER_H
#define AACHISTOGRAM_AACREADER_H

#include "aplayer.h"
#include "aacdec.h"

#include <fstream>
#include <memory>

class aacreader {
public:
    aacreader( char const *filename );
    ~aacreader();

    bool ready() const;
    void readnext();
    uint32_t const * histogram() const
    {
        return m_aacdec_->histogram();
    }

private:
    std::ifstream m_ifile;
    std::shared_ptr< aacdec > m_aacdec_;
    std::shared_ptr< aplayer > m_aplayer_;
    std::vector< uint8_t > m_frame;
};


#endif //AACHISTOGRAM_AACREADER_H
