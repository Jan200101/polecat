add_subdirectory(wine)


set(WINE_JSON_INFILE "${CMAKE_CURRENT_SOURCE_DIR}/runners/wine.json")
set(WINE_JSON_OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/wine_json")


add_custom_command(
    OUTPUT ${WINE_JSON_OUTFILE}.c ${WINE_JSON_OUTFILE}.h
    COMMAND ${CMAKE_COMMAND}
    "-Dbin_in=${WINE_JSON_INFILE}"
    -P ${CMAKE_SOURCE_DIR}/cmake/FileEmbed.cmake
)

add_custom_target(wine_json ALL DEPENDS ${WINE_JSON_OUTFILE}.c ${WINE_JSON_OUTFILE}.h)

set(WINE_MOCK
	${WINE_TAR_OUT}.c
	${WINE_TAR_OUT}.h
)

file(TOUCH ${WINE_MOCK})

SET(MOCK_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/libcurl.c
	${WINE_MOCK}
	${WINE_JSON_OUTFILE}.c
	${WINE_JSON_OUTFILE}.h
)

add_library(mock STATIC ${MOCK_SOURCES})
target_compile_options(mock PUBLIC ${CFLAGS})
target_include_directories(mock PUBLIC ${LIBCURL_INCLUDE_DIRS})
add_dependencies(mock wine_tar wine_json)
target_include_directories(mock PRIVATE ${WINE_TAR_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(NAME ${CMAKE_PROJECT_NAME}_mock)
add_executable(${NAME} ${MAINSOURCES})

target_link_libraries(${NAME} LINK_PUBLIC ${CMAKE_PROJECT_NAME}core)
target_link_libraries(${NAME} LINK_PUBLIC ${JSONC_LIBRARIES})
target_link_libraries(${NAME} LINK_PUBLIC ${LIBARCHIVE_LIBRARIES})
target_link_libraries(${NAME} LINK_PUBLIC mock)

target_compile_options(${NAME} PUBLIC ${CFLAGS})
