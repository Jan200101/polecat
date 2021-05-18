#
# LIBCURL_INCLUDE_DIRS
# LIBCURL_LIBRARIES
# LIBCURL_CFLAGS

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(_CURL libcurl)

	if (BUILD_STATIC AND NOT _CURL_FOUND)
		 message(FATAL_ERROR "Cannot find static build information")
	endif()
endif()

if (_CURL_FOUND) # we can rely on pkg-config
	if (NOT BUILD_STATIC)
		set(LIBCURL_LIBRARIES ${_CURL_LIBRARIES})
		set(LIBCURL_INCLUDE_DIRS ${_CURL_INCLUDE_DIRS})
		set(LIBCURL_CFLAGS ${_CURL_CFLAGS_OTHER})
	else()
		set(LIBCURL_LIBRARIES ${_CURL_STATIC_LIBRARIES})
		set(LIBCURL_INCLUDE_DIRS ${_CURL_STATIC_INCLUDE_DIRS})
		set(LIBCURL_CFLAGS ${_CURL_STATIC_CFLAGS_OTHER})
	endif()
else()
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
		PATHS
			/usr/include /usr/local/include)

	find_library(CURL_LIB
		NAMES curl
		HINTS
			ENV curlPath${_lib_suffix}
			ENV curlPath
		PATHS
			/usr/lib{_lib_suffix} /usr/local/lib{_lib_suffix}
			/usr/lib /usr/local/lib)

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Libcurl DEFAULT_MSG CURL_LIB CURL_INC)
	mark_as_advanced(CURL_INC CURL_LIB)

	if(LIBCURL_FOUND)
		set(LIBCURL_INCLUDE_DIRS ${CURL_INC})
		set(LIBCURL_LIBRARIES ${CURL_LIB})
		set(LIBCURL_CFLAGS "")
	endif()
endif()