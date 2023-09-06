include (${GLOBAL_FFMPEG_DEFINE_CMAKE_FILE})

cmake_minimum_required (VERSION 3.6)
project (02_REMUXING)



add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARYS})