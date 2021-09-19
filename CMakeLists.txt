cmake_minimum_required(VERSION 3.18)
project(meta CXX)

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message("meta: standalone build")
    add_executable(verification verification.cpp iterator_experiments.cpp)
    target_include_directories(verification PRIVATE .)
    target_compile_options(verification
        PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:
            -std=gnu++20
            -fno-exceptions
            -fno-rtti
            -fno-use-cxa-atexit
            -fno-threadsafe-statics
            -Wno-register
            -Wno-volatile
            >
    )
    set_property(TARGET verification PROPERTY CXX_STANDARD 20)
else()
    message("meta: integrated build")
    add_library(meta.iface INTERFACE)
    target_include_directories(meta.iface INTERFACE .)
endif()
