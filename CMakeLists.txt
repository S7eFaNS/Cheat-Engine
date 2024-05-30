cmake_minimum_required(VERSION 3.21.0)
project(Cheat_Engine VERSION 0.1.0 LANGUAGES C CXX)

set (SOURCES
    main.cpp
    Logic/systemFunctionsWindows.cpp
)

add_executable(Cheat_Engine ${SOURCES})

target_include_directories(Cheat_Engine PRIVATE include)