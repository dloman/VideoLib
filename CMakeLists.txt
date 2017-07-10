cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER /usr/local/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++)
set(MODULE_NAME VideoLib)
add_definitions(-DMODULE_NAME=\"VideoLib\")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_definitions(-w)
add_definitions(-fPIC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
add_definitions(-std=c++1z)

project(VideoLib)

find_package(Threads)

find_package(FFmpeg REQUIRED)

include_directories(${FFMPEG_INCLUDE_DIR})

find_package(DanLib REQUIRED HINTS ${DanLib_DIR})
include_directories(${DanLib_INCLUDE_DIR})
message(" - DanLib is subsessfulling included here ${DanLib_INCLUDE_DIR}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(.)

set(CMAKE_DEBUG_POSTFIX d)

add_library(
  VideoPlayerLib
  Utility/Frame.cpp
  VideoPlayer/VideoPlayer.cpp
  )

target_link_libraries(
  VideoPlayerLib
  Image
  ${FFMPEG_LIBRARIES}
  ${FFMPEG_SWSSCALE}
  ${CMAKE_THREAD_LIBS_INIT}
  )

add_executable(
  VideoPlayer
  Test/VideoPlayer.cpp
  )

target_link_libraries(
  VideoPlayer
  VideoPlayerLib
  ${FFMPEG_LIBRARIES}
  )
