cmake_minimum_required(VERSION 3.3)
project(OpenGlSB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${PROJECT_SOURCE_DIR}/cmake})
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(FileList)
set(SOURCE_FILES main.cpp)
add_executable(OpenGlSB ${SRC_FILES}
                        ${HDR_FILES})