#
# JSONC_INCLUDE_DIRS
# JSONC_LIBRARIES
# JSONC_CFLAGS

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(_JSONC json-c)

	if (BUILD_STATIC AND NOT _JSONC_FOUND)
		 message(FATAL_ERROR "Cannot find static build information")
	endif()
endif()

if (_JSONC_FOUND) # we can rely on pkg-config
	if (NOT BUILD_STATIC)
		set(JSONC_LIBRARIES ${_JSONC_LIBRARIES})
		set(JSONC_INCLUDE_DIRS ${_JSONC_INCLUDE_DIRS})
		set(JSONC_CFLAGS ${_JSONC_CFLAGS_OTHER})
	else()
		set(JSONC_LIBRARIES ${_JSONC_STATIC_LIBRARIES})
		set(JSONC_INCLUDE_DIRS ${_JSONC_STATIC_INCLUDE_DIRS})
		set(JSONC_CFLAGS ${_JSONC_STATIC_CFLAGS_OTHER})
	endif()
else()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_lib_suffix 64)
	else()
		set(_lib_suffix 32)
	endif()

	find_path(JSONC_INC
		NAMES json.h
		HINTS
			ENV jsoncPath${_lib_suffix}
			ENV jsoncPath
			${_JSONC_INCLUDE_DIRS}
		PATHS
			/usr/include/json-c /usr/local/include/json-c)

	find_library(JSONC_LIB
		NAMES ${_JSONC_LIBRARIES} jsonc json-c
		HINTS
			ENV jsoncPath${_lib_suffix}
			ENV jsoncPath
			${_JSONC_LIBRARY_DIRS}
			${_JSONC_STATIC_LIBRARY_DIRS}
		PATHS
			/usr/lib{_lib_suffix} /usr/local/lib{_lib_suffix}
			/usr/lib /usr/local/lib)

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(JsonC DEFAULT_MSG JSONC_LIB JSONC_INC)
	mark_as_advanced(JSONC_INC JSONC_LIB)

	if(JSONC_FOUND)
		set(JSONC_INCLUDE_DIRS ${JSONC_INC})
		set(JSONC_LIBRARIES ${JSONC_LIB})
		if (BUILD_STATIC)
			set(JSONC_LIBRARIES ${JSONC_LIBRARIES} ${_JSONC_STATIC_LIBRARIES})
		endif()
	endif()
endif()