cmake_minimum_required(VERSION 3.16)
project(Explosion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    ribbonbar.cpp 
)

add_executable(Explosion ${SOURCES})
target_link_libraries(Explosion PRIVATE Qt6::Widgets)