cmake_minimum_required(VERSION 3.6)

project(FFMPEG_STUDY)

set(GLOBAL_FFMPEG_DEFINE_CMAKE_FILE ${CMAKE_SOURCE_DIR}/globalcmake/ffmpeg.cmake CACHED)

add_subdirectory(01.PrintVersion)