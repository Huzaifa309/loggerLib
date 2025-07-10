cmake_minimum_required(VERSION 3.16)
project(LoggerLib VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
#include_directories(${CMAKE_SOURCE_DIR}/third_party/quill/quill/include)

# Vendor Quill C++
include(FetchContent)

FetchContent_Declare(
    quill
    GIT_REPOSITORY https://github.com/odygrd/quill.git
    GIT_TAG v4.3.0
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/quill
)

# Configure Quill build options
set(QUILL_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(QUILL_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(QUILL_ENABLE_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(QUILL_ENABLE_DOCS OFF CACHE BOOL "" FORCE)
set(QUILL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(quill)

# Create static library
add_library(loggerlib STATIC
    src/logger.cpp
    src/loggerwrapper.cpp
)

# Set library properties
set_target_properties(loggerlib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME loggerlib
)

# Link against Quill
target_link_libraries(loggerlib PUBLIC quill::quill)

# Include directories for the library
target_include_directories(loggerlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler flags
target_compile_features(loggerlib PUBLIC cxx_std_17)

# Installation - install the static library and headers
install(TARGETS loggerlib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Create a simple pkg-config file for easier integration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/loggerlib.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/loggerlib.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/loggerlib.pc
    DESTINATION lib/pkgconfig
)

# Create a simple example if requested
option(BUILD_EXAMPLE "Build example" ON)
if(BUILD_EXAMPLE)
    add_executable(example example/main.cpp)
    target_link_libraries(example loggerlib)
    target_include_directories(example PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        #${CMAKE_SOURCE_DIR}/third_party/quill/quill/include
    )
endif() 