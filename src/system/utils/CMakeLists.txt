cmake_minimum_required(VERSION 3.1)

project(utils LANGUAGES CXX)

message("utils...")

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SysTime.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    )
