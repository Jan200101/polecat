list(APPEND
	MOCK_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/libcurl.c
)

if (ENABLE_WINE)
add_subdirectory(wine)
include_directories(${WINE_MOCK_ARCHIVE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(WINE_MOCK_JSON_INFILE "${CMAKE_CURRENT_SOURCE_DIR}/runners/wine.json")
set(WINE_MOCK_JSON_OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/wine_json")

add_custom_command(
    OUTPUT ${WINE_MOCK_JSON_OUTFILE}.c ${WINE_MOCK_JSON_OUTFILE}.h
    COMMAND ${CMAKE_COMMAND}
    "-Dbin_in=${WINE_MOCK_JSON_INFILE}"
    -P ${CMAKE_SOURCE_DIR}/cmake/FileEmbed.cmake
)

add_custom_target(wine_mock_json ALL DEPENDS ${WINE_MOCK_JSON_OUTFILE}.c ${WINE_MOCK_JSON_OUTFILE}.h)

set(WINE_MOCK
	${WINE_MOCK_ARCHIVE_OUT}.c
	${WINE_MOCK_ARCHIVE_OUT}.h
)

file(TOUCH ${WINE_MOCK})

list(APPEND
	MOCK_SOURCES
	${WINE_MOCK}
	${WINE_MOCK_JSON_OUTFILE}.c
	${WINE_MOCK_JSON_OUTFILE}.h
)

endif()

add_library(mock STATIC ${MOCK_SOURCES})
target_compile_options(mock PUBLIC ${CFLAGS})
target_include_directories(mock PUBLIC ${LIBCURL_INCLUDE_DIRS})

if (ENABLE_WINE)
add_dependencies(mock wine_mock_archive wine_mock_json)
endif()

set(NAME ${CMAKE_PROJECT_NAME}_mock)
add_executable(${NAME} ${SOURCES})

target_link_libraries(${NAME} LINK_PUBLIC ${JSONC_LIBRARIES})
target_link_libraries(${NAME} LINK_PUBLIC ${LIBARCHIVE_LIBRARIES})
target_link_libraries(${NAME} LINK_PUBLIC mock)

target_compile_options(${NAME} PUBLIC ${CFLAGS})
