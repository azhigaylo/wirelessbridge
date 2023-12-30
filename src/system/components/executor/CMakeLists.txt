cmake_minimum_required(VERSION 3.4.0)

project(executor LANGUAGES CXX)

message("executor...")

find_package(Boost REQUIRED COMPONENTS thread system)

if(!Boost_FOUND)
    message(FATAL_ERROR "Boost NOT FOUND")
endif()

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CExecutor.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::boost
    PRIVATE Boost::thread
    PRIVATE loger
    PRIVATE basis
    PRIVATE finalhaven
    )
