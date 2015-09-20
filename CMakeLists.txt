#https://github.com/openglsuperbible/sb6code/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.3)
project(OpenGlSB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${PROJECT_SOURCE_DIR}/cmake})

link_directories(${PROJECT_SOURCE_DIR}/lib)

#add_subdirectory(${PROJECT_SOURCE_DIR}/sb6code-master/sb6code-master/extern/glfw-2.7.6)
include_directories(${PROJECT_SOURCE_DIR}/include/GL)
SET(EXTRA_LIBS "glu32.lib" "opengl32.lib")
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(FileList)
set(SOURCE_FILES main.cpp)



find_package(OpenGL REQUIRED)

add_executable(OpenGlSB ${SRC_FILES}
                        ${HDR_FILES})
target_link_libraries(OpenGlSB glfw OpenGL ${GLFW_LIBRARIES} ${EXTRA_LIBS})