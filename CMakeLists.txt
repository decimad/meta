cmake_minimum_required(VERSION 3.18)
project(meta CXX)

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    # standalone build (verification/tests only)
    add_executable(verification verification.cpp)
    target_include_directories(verification PRIVATE .)

else()
    # we're integrated in a frame project
    add_library(meta.iface INTERFACE)
    target_include_directories(meta.iface INTERFACE .)

endif()
