#include <iostream>

#include "histogram.h"
#include "aacreader.h"

namespace
{
    void show_usage( char const *name )
    {
        std::cerr << "usage: " << name << " [-a -f -s -g -h]\n\n";
        std::cerr << "aac historgram\n\n";
        std::cerr << "options:\n";
        std::cerr << "\t-a, -aac  audio file with aac-adts format\n";
        std::cerr << "\t-f, --font  font to draw axis captions and ticks\n";
        std::cerr << "\t-s, --scale  sum axis scale factor [1-1024] (def. 1)\n";
        std::cerr << "\t-g, --scale  window geometry (def. 512x512)\n";
        std::cerr << "\t-h, --help  show this help message and exit\n";
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
    int scale = 1;
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
                scale = atoi(optarg);
                if( scale == 0 || abs(scale) > aacdec::PACKLEN )
                {
                    show_usage( argv[0]);
                    return 1;
                }
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
    sf::RenderWindow window(sf::VideoMode(viewport), filename, sf::Style::Default);
    window.setPosition(sf::Vector2i(40, 40));
    window.setFramerateLimit( 60 );
    histogram histogram_( viewport,
                          -1 * aacdec::MIN_LEVEL_DB + 1,
                          fontname,
                          scale );

    while( window.isOpen() ) {
        sf::Event event;
        while( window.pollEvent(event) ) {
            switch( event.type ) {
                case sf::Event::KeyReleased: {
                    if( event.key.scancode == sf::Keyboard::Scan::Escape ) {
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
        }
        window.clear(sf::Color::Black);
        window.draw( histogram_ );
        window.display();
    }

    return 0;
}