cmake_minimum_required(VERSION 3.1)

project(events LANGUAGES CXX)

message("events...")

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/EventListener.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::boost
    PRIVATE Boost::thread
    PRIVATE loger
    PRIVATE basis
    )
