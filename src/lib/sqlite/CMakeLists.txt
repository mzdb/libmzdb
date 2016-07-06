project(sqlite)
cmake_minimum_required(VERSION 2.8)

#set( CMAKE_GENERATOR_PLATFORM "Visual Studio 11 2012 Win64")
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_ENABLE_RTREE=1")
set(SQLITE_SRC "sqlite3.c")
add_library(sqlite STATIC ${SQLITE_SRC})
