cmake_minimum_required(VERSION 3.1)

project(loger LANGUAGES CXX)

message("loger...")

find_package(AutomotiveDlt 2.15)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/slog.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${AutomotiveDlt_INCLUDE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    ${AutomotiveDlt_LIBRARY}
    )
