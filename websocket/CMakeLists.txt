cmake_minimum_required (VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 11)

project ("HTTP Server")

find_package(Boost 1.71.0 REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
link_directories(${Boost_LIBRARY_DIRS})

find_package(Threads)

add_executable (HTTPserver
    main.cpp
)

target_include_directories(HTTPserver PUBLIC
                    ${Boost_INCLUDE_DIRS})

target_link_libraries(HTTPserver    
                    ${Boost_LIBRARIES}
                    ${CMAKE_THREAD_LIBS_INIT})













