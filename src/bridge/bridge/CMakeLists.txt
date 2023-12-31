cmake_minimum_required(VERSION 3.4.0)

project(bridge LANGUAGES CXX)

message("bridge...")

# Include 'function(enable_cxx_compiler_flag_if_supported flag)'
include(EnableCxxCompilerFlag)

if(CMAKE_COMPILER_IS_GNUCXX)
    enable_cxx_compiler_flag_if_supported(-fvisibility=hidden)
endif()

find_package(Boost REQUIRED COMPONENTS thread system)
find_package(mosquittopp REQUIRED)

if(Boost_FOUND)
    message("Boost_FOUND..")
else()
    message(FATAL_ERROR "Boost NOT FOUND")
endif()

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CWirelessBridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CDeviceConfig.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${mosquittopp_INCLUDE_DIR}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE loger
    PRIVATE utils
    PRIVATE basis
    PRIVATE events
    PRIVATE finalhaven
    PRIVATE executor
    PRIVATE mqttconn
    PRIVATE Boost::boost
    PRIVATE Boost::thread
    PRIVATE ${mosquittopp_LIBRARY}
    )