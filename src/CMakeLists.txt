set(C_STANDARD 99)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/command.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dxvk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dxvk.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lutris.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/main.h
    ${CMAKE_CURRENT_SOURCE_DIR}/net.c
    ${CMAKE_CURRENT_SOURCE_DIR}/net.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tar.h
)


if (NOT WIN32)
set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/wine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wine.h
)
endif()

find_package(Libcurl REQUIRED)
find_package(JsonC REQUIRED)
find_package(Libarchive REQUIRED)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${JSONC_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${LIBARCHIVE_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${LIBCURL_LIBRARIES})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${JSONC_INCLUDE_DIRS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${LIBARCHIVE_INCLUDE_DIRS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${LIBCURL_INCLUDE_DIRS})

set(CFLAGS
    -Wall -Wextra -pedantic
    -Wconversion -Wshadow -Wstrict-aliasing
    -Winit-self -Wcast-align -Wpointer-arith
    -Wmissing-declarations -Wmissing-include-dirs
    -Wno-unused-parameter -Wuninitialized
    ${JSONC_CFLAGS}
    ${LIBARCHIVE_CFLAGS}
    ${LIBCURL_CFLAGS}
)

target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC ${CFLAGS})

install(TARGETS ${CMAKE_PROJECT_NAME})

if(BUILD_MOCK)
    add_subdirectory(mock)
endif()