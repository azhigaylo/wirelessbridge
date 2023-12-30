cmake_minimum_required(VERSION 3.1)

project(basis LANGUAGES CXX)

message("basis...")

add_library(${PROJECT_NAME} SHARED
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
    PRIVATE ${CMAKE_DL_LIBS}
    PRIVATE loger
    PRIVATE Boost::boost
    PRIVATE Boost::system
    PRIVATE Boost::filesystem
    )
