cmake_minimum_required(VERSION 3.15)

# Project configuration
project(MacBookLidAngle
    VERSION 1.0.0
    DESCRIPTION "C++ library for reading MacBook lid angle sensor"
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Only build on macOS
if(NOT APPLE)
    message(FATAL_ERROR "This library only supports macOS")
endif()

# Find required frameworks
find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

# Library source files
set(SOURCES
    angle.cpp
)

set(HEADERS
    angle.h
)

# Create the library
add_library(lid_angle STATIC ${SOURCES} ${HEADERS})

# Set target properties
target_include_directories(lid_angle
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Link required frameworks
target_link_libraries(lid_angle
    PRIVATE
        ${IOKIT_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
)

# Compiler flags
target_compile_options(lid_angle PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
)

# Enable release optimizations by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Example program (optional)
option(BUILD_EXAMPLE "Build example program" ON)
if(BUILD_EXAMPLE)
    add_executable(lid_angle_example example.cpp)
    target_link_libraries(lid_angle_example lid_angle)
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS lid_angle
    EXPORT MacAngleTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets for find_package
install(EXPORT MacAngleTargets
    FILE MacAngleTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MacAngle
)

# Create config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/MacAngleConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/MacAngleConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MacAngle
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/MacAngleConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MacAngleConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MacAngleConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MacAngle
)
