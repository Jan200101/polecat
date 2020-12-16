#
# JSONC_INCLUDE_DIRS
# JSONC_LIBRARIES

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(_JSONC QUIET json-c)
endif()

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
	PATHS
		/usr/lib /usr/local/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JsonC DEFAULT_MSG JSONC_LIB JSONC_INC)
mark_as_advanced(JSONC_INC JSONC_LIB)

if(JSONC_FOUND)
	set(JSONC_INCLUDE_DIRS ${JSONC_INC})
	set(JSONC_LIBRARIES ${JSONC_LIB})
endif()