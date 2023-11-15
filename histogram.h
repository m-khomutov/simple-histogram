//
// Created by mkh on 07.11.2023.
//

#ifndef AACHISTOGRAM_HISTOGRAM_H
#define AACHISTOGRAM_HISTOGRAM_H

#include "coords.h"

class histogram: public sf::Drawable, public sf::Transformable {
public:
    histogram( sf::Vector2u const& vp, size_t count, const char* fontname, int scale );
    void update( uint32_t const *data );

private:
    sf::Vector2u m_viewport;
    sf::VertexArray m_vertices;
    size_t m_scaler;
    coords m_coords;

private:
    void draw( sf::RenderTarget& target, const sf::RenderStates& states) const override;
};


#endif //AACHISTOGRAM_HISTOGRAM_H
