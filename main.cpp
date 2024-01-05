#include <iostream>

#include "histogram.h"
#include "aacreader.h"

namespace
{
    void show_usage( char const *name )
    {
        std::cerr << "usage: " << name << " [-a -f -s -g -h]\n\n";
        std::cerr << "plays AAC data inside ADTS frame and draws its histogram\n\n";
        std::cerr << "options:\n";
        std::cerr << "\t-a, audio file with aac-adts format\n";
        std::cerr << "\t-f, font to draw axis captions and ticks\n";
        std::cerr << "\t-s, sum axis scale factor (def. 1.)\n";
        std::cerr << "\t-g, window geometry (def. 512x512)\n";
        std::cerr << "\t-h, show this help message and exit\n";
    }
    bool parseGeometry( char const *arg, std::pair<size_t, size_t> *rc ) {
        char *eptr {};
        rc->first = strtol(arg, &eptr, 10);
        if( !rc->first || *eptr != 'x' )
        {
            return false;
        }
        rc->second = strtol(eptr + 1, nullptr, 10);
        return rc->second != 0;
    }
}  //namespace

int main( int argc, char* argv[] )
{
    float scale = 1.f;
    char const *fontname = nullptr;
    char const *filename = nullptr;
    std::pair< size_t , size_t > geometry( 512, 512);
    int c;
    while( (c = getopt (argc, argv, "a:s:f:g:h")) != -1)
    {
        switch (c)
        {
            case 'a':
                filename = optarg;
                break;
            case 's':
                scale = atof(optarg);
                break;
            case 'f':
                fontname = optarg;
                break;
            case 'g':
                if( !parseGeometry( optarg, &geometry) )
                {
                    show_usage( argv[0]);
                    return 1;
                }
                break;
            case 'h':
            default:
                show_usage( argv[0]);
                return 1;
        }
    }
    if( ! fontname )
    {
        show_usage( argv[0]);
        return 1;
    }

    aacreader r( filename );
    sf::Vector2u viewport( geometry.first, geometry.second );
    sf::RenderWindow window(sf::VideoMode(viewport.x, viewport.y), filename, sf::Style::Default);
    window.setPosition(sf::Vector2i(40, 40));
    window.setFramerateLimit( 60 );
    histogram histogram_( viewport,
                          r.histogram_size(),
                          fontname,
                          scale );

    while( window.isOpen() ) {
        sf::Event event;
        while( window.pollEvent(event) ) {
            switch( event.type ) {
                case sf::Event::KeyReleased: {
                    if( event.key.code == sf::Keyboard::Escape ) {
                        window.close();
                    }
                    break;
                }
                case sf::Event::Closed:
                    window.close();
                    break;
            }
        }
        try {
            if( r.ready() ) {
                r.readnext();
                histogram_.update( r.histogram() );
            }
        }
        catch( const std::logic_error& err ) {
            std::cerr << err.what() << std::endl;
            window.close();
            break;
        }
        window.clear(sf::Color::Black);
        window.draw( histogram_ );
        window.display();
    }

    return 0;
}
