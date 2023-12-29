cmake_minimum_required(VERSION 3.1)

project(basis LANGUAGES CXX)

message("basis...")

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers/src/CComponentConfigParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/managers/src/CComponentLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/managers/src/CLoaderFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/managers/src/CComponentManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/managers/src/ComponentManagerFactory.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/helpers/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/component/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/interface/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/managers/include
    )

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_DL_LIBS}
    loger
    Boost::boost
    Boost::system
    Boost::filesystem
    )
