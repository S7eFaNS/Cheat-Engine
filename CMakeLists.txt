cmake_minimum_required(VERSION 3.21.0)
project(Cheat_Engine VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Cheat_Engine main.cpp)
target_include_directories(Cheat_Engine PRIVATE include)

target_sources(Cheat_Engine PRIVATE Logic/systemFunctionsWindows.cpp)