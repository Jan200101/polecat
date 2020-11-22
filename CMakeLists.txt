cmake_minimum_required(VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

project(polecat VERSION 0.1.8)

add_compile_definitions(NAME="${CMAKE_PROJECT_NAME}")
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")

message(STATUS "Name: ${CMAKE_PROJECT_NAME}")
message(STATUS "Version: ${CMAKE_PROJECT_VERSION}")

set(FILES common config dxvk lutris main net tar wine)

foreach(FILE ${FILES})
	set(SOURCES ${SOURCES} ${CMAKE_CURRENT_LIST_DIR}/src/${FILE}.c ${CMAKE_CURRENT_LIST_DIR}/src/${FILE}.h)
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

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${LIBCURL_INCLUDE_DIRS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${JSONC_INCLUDE_DIRS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${LIBARCHIVE_INCLUDE_DIRS})

install(TARGETS ${CMAKE_PROJECT_NAME})