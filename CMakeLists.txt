cmake_minimum_required(VERSION 3.16)

# Project setup
set(PROJECT_NAME quillLogger)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Vendor Quill (includes bundled fmt)
add_subdirectory(third_party/quill)

# Logger library
add_library(${PROJECT_NAME} STATIC src/sharded_logging.cpp)

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE quill)

# Public include paths
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/quill/quill/include>
    $<INSTALL_INTERFACE:include>
)

# Install headers + static lib
install(DIRECTORY include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY third_party/quill/quill/include/ DESTINATION include)

# Example target (optional)
option(BUILD_EXAMPLE "Build example program" ON)
if(BUILD_EXAMPLE)
    add_executable(example example/main.cpp)
    target_link_libraries(example PRIVATE ${PROJECT_NAME} pthread)
endif()
