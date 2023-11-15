cmake_minimum_required(VERSION 3.20)
project(simple_histogram)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_histogram main.cpp adts.cpp aacdec.cpp aplayer.cpp aacreader.cpp histogram.cpp coords.cpp)
target_link_libraries(simple_histogram
                      avformat avcodec avutil swresample swscale
                      asound
                      sfml-system sfml-window sfml-graphics)
