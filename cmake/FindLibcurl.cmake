#
# LIBCURL_INCLUDE_DIRS
# LIBCURL_LIBRARIES

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(_CURL QUIET curl)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(_lib_suffix 64)
else()
	set(_lib_suffix 32)
endif()

find_path(CURL_INC
	NAMES curl/curl.h
	HINTS
		ENV curlPath${_lib_suffix}
		ENV curlPath
		${_CURL_INCLUDE_DIRS}
	PATHS
		/usr/include /usr/local/include)

find_library(CURL_LIB
	NAMES ${_CURL_LIBRARIES} curl libcurl
	HINTS
		ENV curlPath${_lib_suffix}
		ENV curlPath
		${_CURL_LIBRARY_DIRS}
	PATHS
		/usr/lib /usr/local/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libcurl DEFAULT_MSG CURL_LIB CURL_INC)
mark_as_advanced(CURL_INC CURL_LIB)

if(LIBCURL_FOUND)
	set(LIBCURL_INCLUDE_DIRS ${CURL_INC})
	set(LIBCURL_LIBRARIES ${CURL_LIB})
endif()