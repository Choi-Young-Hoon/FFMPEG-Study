cmake_minimum_required(VERSION 3.6)

project(FFMPEG_STUDY)

set(CMAKE_BUILD_TYPE Debug)
set(GLOBAL_FFMPEG_DEFINE_CMAKE_FILE ${CMAKE_SOURCE_DIR}/globalcmake/global.cmake CACHED)

add_subdirectory(01.PrintVersion)
add_subdirectory(02.Remuxing)