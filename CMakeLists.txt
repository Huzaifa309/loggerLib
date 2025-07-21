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

# Vendor Quill and its bundled fmt
add_subdirectory(third_party/quill)

# Logger is now header-only (logger.h)
# Only loggerwrapper.cpp needs to be compiled
add_library(loggerlib STATIC
    src/loggerwrapper.cpp
)

# Link Quill (which brings in bundled fmt)
target_link_libraries(loggerlib PRIVATE quill)

# Install headers and static library
install(DIRECTORY include/ DESTINATION include)
install(TARGETS loggerlib DESTINATION lib)
install(DIRECTORY third_party/quill/quill/include/ DESTINATION include)

# Example target (optional)
option(BUILD_EXAMPLE "Build example program" ON)
if(BUILD_EXAMPLE)
    include_directories(${CMAKE_SOURCE_DIR}/third_party/quill/quill/include)
    add_executable(example example/main.cpp)
    target_link_libraries(example PRIVATE loggerlib pthread)
endif()

# Note: Users do NOT need to link to system fmt or quill. All dependencies are vendored and linked via loggerlib. 

target_include_directories(loggerlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/quill/quill/include>
    $<INSTALL_INTERFACE:include>
) 