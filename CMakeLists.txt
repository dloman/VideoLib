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

SET(VideoLib_VERSION_MAJOR 0)
SET(VideoLib_VERSION_MINOR 1)

###############################################################################
# Dependencies
###############################################################################
find_package(Threads)

find_package(FFmpeg REQUIRED)

include_directories(${FFMPEG_INCLUDE_DIR})

find_package(DanLib REQUIRED HINTS ${DanLib_DIR})
include_directories(${DanLib_INCLUDE_DIR})
message(" - DanLib is subsessfulling included here ${DanLib_INCLUDE_DIR}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(.)

set(CMAKE_DEBUG_POSTFIX d)

###############################################################################
# Libraries
###############################################################################
add_library(
  VideoLibUtility
  VideoLib/Utility/Frame.cpp
  VideoLib/Utility/Packet.cpp
  )

target_link_libraries(
  VideoLibUtility
  ${FFMPEG_LIBRARIES}
  )

add_library(
  VideoPlayerLib
  VideoLib/VideoPlayer/VideoPlayer.cpp
  )

target_link_libraries(
  VideoPlayerLib
  VideoLibUtility
  Image
  ${CMAKE_THREAD_LIBS_INIT}
  )

add_library(
  VideoStreamerLib
  VideoLib/VideoStreamer/VideoStreamer.cpp
  )

target_link_libraries(
  VideoStreamerLib
  VideoLibUtility
  Image
  ${CMAKE_THREAD_LIBS_INIT}
  )

###############################################################################
# Tests
###############################################################################
#if (ENABLE_TESTS)

  find_package(OpenCV REQUIRED)

  include_directories(${OpenCV_INCLUDE_DIR})

  add_executable(
    VideoPlayer
    Test/VideoPlayerTest.cpp
  )

  target_link_libraries(
    VideoPlayer
    VideoPlayerLib
    ${FFMPEG_LIBRARIES}
    ${OpenCV_LIBRARIES}
  )

  add_executable(
    VideoStreamer
    Test/VideoStreamerTest.cpp
    )
  target_link_libraries(
    VideoStreamer
    VideoStreamerLib
    )
#endif()

###############################################################################
# Install
###############################################################################
set(VideoLib_VERSION_STRING "${VideoLib_VERSION_MAJOR}.${VideoLib_VERSION_MINOR}")
set(VideoLib_DIRNAME        "VideoLib-${VideoLib_VERSION_STRING}")

if (MSVC AND CMAKE_CL_64)
  set(VideoLib_DIRNAME_lib "lib/${VideoLib_DIRNAME}/x64")
elseif (MSVC)
  set(VideoLib_DIRNAME_lib "lib/${VideoLib_DIRNAME}/x86")
else ()
  set(VideoLib_DIRNAME_lib "lib/${VideoLib_DIRNAME}")
endif ()

set(VideoLib_DIRNAME_include "include/${VideoLib_DIRNAME}")

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VideoLibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/VideoLibConfig.cmake"
  INSTALL_DESTINATION
    ${VideoLib_DIRNAME_lib}
  PATH_VARS
    VideoLib_DIRNAME_lib
    VideoLib_DIRNAME_include
  )

WRITE_BASIC_PACKAGE_VERSION_FILE(
  "${CMAKE_CURRENT_BINARY_DIR}/VideoLibConfigVersion.cmake"
  VERSION ${VideoLib_VERSION_STRING}
  COMPATIBILITY AnyNewerVersion
  )

install(
TARGETS
  VideoLibUtility
  VideoPlayerLib
  VideoStreamerLib
DESTINATION
  ${VideoLib_DIRNAME_lib}
EXPORT
  VideoLibExports
  )

install(
  EXPORT
    VideoLibExports
  DESTINATION
    ${VideoLib_DIRNAME_lib}
  )

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/VideoLibConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/VideoLibConfigVersion.cmake
  DESTINATION
    ${VideoLib_DIRNAME_lib}
  )

install(
  FILES
    VideoLib/VideoPlayer/VideoPlayer.hpp
  DESTINATION
    ${VideoLib_DIRNAME_include}/VideoLib/VideoPlayer
  )

install(
  FILES
    VideoLib/Utility/Frame.hpp
    VideoLib/Utility/Packet.hpp
  DESTINATION
    ${VideoLib_DIRNAME_include}/VideoLib/Utility
  )
