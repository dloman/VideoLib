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

message("what the fuck why isnt this working your funking fuck${FFMPEG_LIBRARIES}")

#add_library(
#H264Encoder
#src/H264Encoder.cpp
#src/H264Encoder.hpp
#src/Packet.cpp
#src/Frame.cpp
#)
#
#target_link_libraries(
#H264Encoder
#${FFMPEG_LIBRARIES}
#)
#
#add_executable(
#H264FileTest
#src/Test/H264FileTest.cpp
#)
#
#target_link_libraries(
#H264FileTest
#H264Encoder
#${CMAKE_THREAD_LIBS_INIT}
#)
#
#add_library(
#RtspStreamer
#src/RtspFramedSource.cpp
#src/RtspFramedSource.hpp
#src/RtspServer.cpp
#src/RtspServer.hpp
#src/RtspSubsession.cpp
#src/RtspSubsession.hpp
#)
#
#target_link_libraries(
#RtspStreamer
#${Live555_LIBRARIES}
#)

add_library(
  VideoPlayerLib
  VideoPlayer/VideoPlayer.cpp
  )

target_link_libraries(
  VideoPlayerLib
  ${FFMPEG_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  )
#[[
add_executable(
  VideoBroadcaster
  src/Main.cpp
  src/H264FramedSource.cpp
  )

target_link_libraries(
  VideoBroadcaster
  #H264Encoder
  #RtspStreamer
  ${FFMPEG_LIBRARIES}
  ${Live555_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  )
#]]
