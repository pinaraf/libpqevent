cmake_minimum_required(VERSION 2.6)
project(libpqevent)

add_executable(libpqevent main.cpp)

install(TARGETS libpqevent RUNTIME DESTINATION bin)
