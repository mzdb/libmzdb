project(libmzdb)
cmake_minimum_required(VERSION 2.8)

### Export all symbols when creating the .dll file
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

### Filters used to include files in the build
#aux_source_directory(. SRC_LIST) # defined in default CMakeLists file
file(GLOB SRC_LIST "*.c")
file(GLOB HEADERS "*.h")
add_subdirectory(lib/sqlite)
#add_subdirectory(lib/expat-2.1.1)
add_subdirectory(lib/xmlbare)

include_directories(HEADERS)
set( MY_SRCS ${SRC_LIST} ${HEADERS})

## Command that must be used instead of add_library to build a .exe file
#add_executable(${PROJECT_NAME} ${MY_SRCS} ) # to compile the EXE file

### Command that must be used instead of add_executable to build a .dll file
add_library(libmzdb SHARED ${MY_SRCS}) # to compile the DLL file
include(GenerateExportHeader)
generate_export_header(libmzdb)

### Specify targets for linker
target_link_libraries(libmzdb sqlite xmlbare)

### Other misc CMAKE flags
set( CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Wall") # display all warnings
set( CMAKE_VERBOSE_MAKEFILE on ) # enable verbose makefile
#set( CMAKE_GENERATOR_PLATFORM "Visual Studio 11 2012 Win64")
