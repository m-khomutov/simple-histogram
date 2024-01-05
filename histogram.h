//
// Created by mkh on 07.11.2023.
//

#ifndef AACHISTOGRAM_HISTOGRAM_H
#define AACHISTOGRAM_HISTOGRAM_H

#include "coords.h"

class histogram: public sf::Drawable, public sf::Transformable {
public:
    histogram( sf::Vector2u const& vp, size_t count, const char* fontname, float scale );
    void update( uint64_t const *data );

private:
    sf::Vector2u m_viewport;
    sf::VertexArray m_vertices;
    float m_scaler;
    float m_coef_x;
    float m_coef_y;
    coords m_coords;

private:
    void draw( sf::RenderTarget& target, sf::RenderStates states) const override;
};


#endif //AACHISTOGRAM_HISTOGRAM_H
