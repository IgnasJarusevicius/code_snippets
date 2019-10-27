########################################################################
## Create lime-suite project
########################################################################
cmake_minimum_required(VERSION 3.1.3)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE TYPE INTERNAL FORCE)
project("OGL")

list(FIND CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}" index)
if (${index} EQUAL -1)
   set(CMAKE_BUILD_TYPE "Release")
endif()

#C++11 is a required language feature for this project
set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLM REQUIRED glm)
find_package(GLEW REQUIRED)
add_executable(oglTest main.cpp fft.cpp glad/glad.c)

include_directories(${GLEW_INCLUDE_DIRS}
                    ${GLFW_INCLUDE_DIRS}
                    ${GLM_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(oglTest 
                    ${OPENGL_LIBRARIES}
                    ${GLFW_LIBRARIES}
                    ${GLEW_LIBRARIES}
                    dl)






