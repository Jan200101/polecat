SET(CURL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/libcurl.c
)

add_library(mockcurl STATIC ${CURL_SOURCES})
target_compile_options(mockcurl PUBLIC ${CFLAGS})
target_include_directories(mockcurl PUBLIC ${LIBCURL_INCLUDE_DIRS})

set(NAME ${CMAKE_PROJECT_NAME}_mock)
add_executable(${NAME} ${SOURCES})

target_link_libraries(${NAME} LINK_PUBLIC ${JSONC_LIBRARIES})
target_link_libraries(${NAME} LINK_PUBLIC ${LIBARCHIVE_LIBRARIES})
target_link_libraries(${NAME} LINK_PUBLIC mockcurl)

target_include_directories(${NAME} PUBLIC ${JSONC_INCLUDE_DIRS})
target_include_directories(${NAME} PUBLIC ${LIBARCHIVE_INCLUDE_DIRS})
target_include_directories(${NAME} PUBLIC ${LIBCURL_INCLUDE_DIRS})

target_compile_options(${NAME} PUBLIC ${CFLAGS})