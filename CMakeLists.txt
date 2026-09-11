cmake_minimum_required(VERSION 3.21)

project(BasketballLeagueSystem
    VERSION 1.0.0
    DESCRIPTION "Basketball League Management System (Qt Widgets)"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Svg)

qt_standard_project_setup()

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
list(APPEND PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc")

qt_add_executable(BasketballLeagueSystem
    WIN32
    ${PROJECT_SOURCES}
)

target_include_directories(BasketballLeagueSystem PRIVATE src)

target_link_libraries(BasketballLeagueSystem PRIVATE
    Qt6::Widgets
    Qt6::Svg
)

# MSVC: 源码含中文，必须以 UTF-8 解析
if(MSVC)
    target_compile_options(BasketballLeagueSystem PRIVATE /utf-8 /W3)
endif()

set_target_properties(BasketballLeagueSystem PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# 输出到 build 目录下集中管理
set_target_properties(BasketballLeagueSystem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
