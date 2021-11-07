
find_package(Libcurl REQUIRED)
find_package(JsonC REQUIRED)
find_package(Libyaml REQUIRED)
find_package(Libarchive REQUIRED)

include_directories(${LIBCURL_INCLUDE_DIRS})
include_directories(${JSONC_INCLUDE_DIRS})
include_directories(${LIBYAML_INCLUDE_DIRS})
include_directories(${LIBARCHIVE_INCLUDE_DIRS})

list(APPEND
    CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/command.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/main.h
    ${CMAKE_CURRENT_SOURCE_DIR}/net.c
    ${CMAKE_CURRENT_SOURCE_DIR}/net.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tar.h
)

if(ENABLE_DXVK)
add_compile_definitions(DXVK_ENABLED)
add_library(dxvk OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/dxvk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dxvk.h
)

list(APPEND
    SOURCES
    $<TARGET_OBJECTS:dxvk>
)
endif()

if(ENABLE_LUTRIS)
add_compile_definitions(LUTRIS_ENABLED)
add_library(lutris OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris/array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris/enum.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris/keyword.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris/runner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris/task.h
)

list(APPEND
    SOURCES
    $<TARGET_OBJECTS:lutris>
)
endif()

if(ENABLE_BOTTLES)
add_compile_definitions(BOTTLES_ENABLED)
add_library(bottles OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/bottles.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bottles.h
)

list(APPEND
    SOURCES
    $<TARGET_OBJECTS:bottles>
)
endif()

if (ENABLE_WINE AND NOT WIN32)
add_compile_definitions(WINE_ENABLED)
add_library(wine OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/wine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wine.h
)

list(APPEND
    SOURCES
    $<TARGET_OBJECTS:wine>
)
endif()

set(CFLAGS
    -Wall -Wextra -pedantic
    -Wconversion -Wshadow -Wstrict-aliasing
    -Winit-self -Wcast-align -Wpointer-arith
    -Wmissing-declarations -Wmissing-include-dirs
    -Wno-unused-parameter -Wuninitialized
    ${LIBCURL_CFLAGS}
    ${JSONC_CFLAGS}
    ${LIBYAML_CFLAGS}
    ${LIBARCHIVE_CFLAGS}
)

add_library(core OBJECT ${CORE_SOURCES})
list(APPEND
    SOURCES
    $<TARGET_OBJECTS:core>
)

target_compile_options(core PUBLIC ${CFLAGS})

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${LIBCURL_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${JSONC_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${LIBYAML_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${LIBARCHIVE_LIBRARIES})

install(TARGETS ${CMAKE_PROJECT_NAME})

if(BUILD_MOCK)
    add_subdirectory(mock)
endif()
