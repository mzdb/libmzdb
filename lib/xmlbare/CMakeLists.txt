project(xmlbare)
cmake_minimum_required(VERSION 2.8)

set(XMLBARE_SRC "parser.c")
#set( CMAKE_GENERATOR_PLATFORM "Visual Studio 11 2012 Win64")
add_library(xmlbare STATIC ${XMLBARE_SRC})
