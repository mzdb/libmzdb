project(libmzdb)
cmake_minimum_required(VERSION 2.8)
#aux_source_directory(. SRC_LIST) # defined in default CMakeLists file
file(GLOB SRC_LIST "*.c")
file(GLOB HEADERS "*.h")
add_subdirectory(lib/sqlite)
#add_subdirectory(lib/expat-2.1.1)
add_subdirectory(lib/xmlbare)

include_directories(HEADERS)
set( MY_SRCS ${SRC_LIST} ${HEADERS})

#add_library(libmzdb SHARED ${MY_SRCS}) # to compile the DLL file
add_executable(${PROJECT_NAME} ${MY_SRCS} ) # to compile the EXE file

target_link_libraries(libmzdb sqlite xmlbare)

set( CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Wall")
set( CMAKE_VERBOSE_MAKEFILE on )
#set( CMAKE_GENERATOR_PLATFORM "Visual Studio 11 2012 Win64")
