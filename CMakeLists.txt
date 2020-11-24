cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

project (Portals)

# Find libs
find_library(ALLEGRO_OGG_LIBRARY NAMES liblogg)
find_library(ALLEGRO_LIBRARY NAMES liballeg44)
find_library(ALLEGRO_PNG_LIBRARY NAMES libloadpng)

find_library(OGG_LIBRARY NAMES libogg)
find_library(VORBIS_FILE_LIBRARY NAMES libvorbisfile)
find_library(VORBIS_LIBRARY NAMES libvorbis)

find_library(PNG_LIBRARY NAMES libpng16)
find_library(ZLIB_LIBRARY NAMES libz)

# Source code
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_executable (Portals ${SOURCES} ${HEADERS})

target_compile_options(Portals PRIVATE -Wall -Wextra -pedantic)

# Link Libs

target_link_libraries(Portals ${ALLEGRO_LIBRARY})
target_link_libraries(Portals ${ALLEGRO_PNG_LIBRARY})
target_link_libraries(Portals ${ALLEGRO_OGG_LIBRARY})

target_link_libraries(Portals ${OGG_LIBRARY})
target_link_libraries(Portals ${VORBIS_FILE_LIBRARY})
target_link_libraries(Portals ${VORBIS_LIBRARY})

target_link_libraries(Portals ${ZLIB_LIBRARY})
target_link_libraries(Portals ${PNG_LIBRARY})

target_link_libraries(Portals -static-libgcc -static-libstdc++)