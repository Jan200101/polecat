cmake_minimum_required(VERSION 3.12)

if(CMAKE_POLICY_DEFAULT_CMP0017 OR CMAKE_POLICY_DEFAULT_CMP0020)
	# touch these to remove warnings
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

option(BUILD_MOCK "build polecat against mock libraries" OFF)
option(ENABLE_WINE "enables wine support [Not on Windows]" ON)
option(ENABLE_DXVK "enables dxvk support" ON)
option(ENABLE_LUTRIS "enables lutris support" OFF)
option(ENABLE_BOTTLES "enables bottles support" ON)

project(polecat VERSION 0.2.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_definitions(NAME="${CMAKE_PROJECT_NAME}")
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")

message(STATUS "Name: ${CMAKE_PROJECT_NAME}")
message(STATUS "Version: ${CMAKE_PROJECT_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)