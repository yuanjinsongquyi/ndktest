
cmake_minimum_required(VERSION 3.4.1)

file(GLOB source_file src/main/cpp/*.cpp)
add_library(
             native-lib


             SHARED

             ${source_file} )


find_library(
              log-lib

              log )
include_directories(src/main/cpp/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS -L${CMAKE_SOURCE_DIR}/src/main/cpp/lib/")

target_link_libraries(
                       native-lib
                       avformat avcodec avfilter  z  avutil swresample swscale
                        android OpenSLES
                       ${log-lib} )