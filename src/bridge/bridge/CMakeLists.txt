cmake_minimum_required(VERSION 3.4.0)

project(bridge LANGUAGES CXX)

message("bridge...")

find_package(Boost REQUIRED COMPONENTS thread system)
find_package(mosquittopp REQUIRED)

if(Boost_FOUND)
    message("Boost_FOUND..")
else()
    message(FATAL_ERROR "Boost NOT FOUND")
endif()

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CWirelessBridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CDeviceConfigParser.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${mosquittopp_INCLUDE_DIR}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    basis
    common
    finalhaven
    executor
    mqttconn
    Boost::boost
    Boost::thread
    ${mosquittopp_LIBRARY}
    )