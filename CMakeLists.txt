cmake_minimum_required(VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

project(polecat VERSION 0.1.5)

add_compile_definitions(NAME="${CMAKE_PROJECT_NAME}")
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")

message("-- Name: ${CMAKE_PROJECT_NAME}\n-- Version: ${CMAKE_PROJECT_VERSION}")

set(FILES common config dxvk lutris main net tar wine)

foreach(FILE ${FILES})
	set(SOURCES ${SOURCES} ${CMAKE_CURRENT_LIST_DIR}/src/${FILE}.c)
endforeach()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Libcurl REQUIRED)
find_package(JsonC REQUIRED)
find_package(Libarchive REQUIRED)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_compile_definitions(DEBUG)
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${LIBCURL_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${JSONC_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${LIBARCHIVE_LIBRARIES})