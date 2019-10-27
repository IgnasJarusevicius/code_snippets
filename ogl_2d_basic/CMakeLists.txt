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

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCXX)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    	add_compile_options(-Wall -Wpedantic)
    endif()
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLM REQUIRED glm)
add_executable(oglTest 
                main.cpp
                BaseObj.cpp
                TextObj.cpp
                AnimObj.cpp
                Picture.cpp
                Rectangle.cpp)

include_directories(${GLEW_INCLUDE_DIRS}
                    ${GLFW_INCLUDE_DIRS}
                    ${GLM_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(oglTest
                    ${OPENGL_LIBRARIES}
                    ${GLEW_LIBRARIES}
                    ${GLFW_LIBRARIES})

if (UNIX)
    add_custom_target(link_img ALL COMMAND ${CMAKE_COMMAND} -E create_symlink  ${CMAKE_CURRENT_SOURCE_DIR}/img ${CMAKE_BINARY_DIR}/img)
endif(UNIX)






