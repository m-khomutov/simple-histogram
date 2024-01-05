cmake_minimum_required(VERSION 3.20)
project(simple_histogram)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB DECODER_SOURCES
    faad/*.h faad/*.c
)

add_executable(simple_histogram
               main.cpp
               adts.cpp
               aacdec.cpp
               aplayer.cpp
               aacreader.cpp
               histogram.cpp
               coords.cpp
               ${DECODER_SOURCES})
target_link_libraries(simple_histogram
                      asound
                      sfml-system sfml-window sfml-graphics)
