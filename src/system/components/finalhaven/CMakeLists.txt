cmake_minimum_required(VERSION 3.4.0)

project(finalhaven LANGUAGES CXX)

message("finalhaven...")

# Include 'function(enable_cxx_compiler_flag_if_supported flag)'
include(EnableCxxCompilerFlag)

if(CMAKE_COMPILER_IS_GNUCXX)
    enable_cxx_compiler_flag_if_supported(-fvisibility=hidden)
endif()

find_package(Boost REQUIRED COMPONENTS thread system)

if(!Boost_FOUND)
    message(FATAL_ERROR "Boost NOT FOUND")
endif()

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CFinalHaven.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::boost
    PRIVATE Boost::thread
    PRIVATE loger
    PRIVATE basis
    )
