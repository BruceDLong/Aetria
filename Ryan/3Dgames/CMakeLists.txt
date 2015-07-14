cmake_minimum_required(VERSION 2.8.5)
cmake_policy(SET CMP0015 NEW)

#find_package(Boost  COMPONENTS thread date_time system REQUIRED)

add_definitions(-std=gnu++11 -Wno-deprecated -Wno-write-strings -fpermissive -g -Wall -pthread ) # `pkg-config --cflags glib-2.0`)

#set(UNICODE_LIBS icuio icui18n icutu icuuc icudata icule iculx icutest dl)

set(PANGO_INCLUDE_DIRS /usr/include/pango-1.0/ /usr/include/cairo /usr/include/glib-2.0/
                       /usr/lib/x86_64-linux-gnu/glib-2.0/include
                        /usr/include/pixman-1
					   /usr/include/freetype2 /usr/include/libpng12)
set(PANGO_LIBS pangocairo-1.0 pango-1.0 cairo gobject-2.0 gmodule-2.0 gthread-2.0 rt glib-2.0)

include_directories( ${PANGO_INCLUDE_DIRS}
                     /usr/local/include/OGRE/
                     $ENV{HOME}/myBin/ogitor/Dependencies/PagedGeometry/include
                     $ENV{HOME}/ourGame/Ryan/ogre-procedural/library/include/
                     )

#link_directories(
#				 ${HomeDir}/../core
#                 ${HomeDir}/../builds/libs/SDL_image/lib/
#                 ${HomeDir}/../builds/libs/SDL/1.3/lib
#				)

#add_executable(Text TextGui.cpp )
#target_link_libraries(Text SDL2_image SDL2 ${PANGO_LIBS})

add_executable(game main.cpp)
target_link_libraries(game SDL2_mixer SDL2_image SDL2 ${PANGO_LIBS} boost_system
        OgreMain
        OgreOverlay
        "${CMAKE_CURRENT_BINARY_DIR}/libOgreProcedural.so"
  #      $ENV{HOME}/myBin/ogitor-build/Dependencies/lib/libPagedGeometry.a
        OgreTerrain
        pthread
        )
