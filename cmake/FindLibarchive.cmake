#
# LIBARCHIVE_INCLUDE_DIRS
# LIBARCHIVE_LIBRARIES
# LIBARCHIVE_CFLAGS

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(_LIBARCHIVE libarchive)

	if (BUILD_STATIC AND NOT _LIBARCHIVE_FOUND)
		 message(FATAL_ERROR "Cannot find static build information")
	endif()
endif()

if (_LIBARCHIVE_FOUND) # we can rely on pkg-config
	if (NOT BUILD_STATIC)
		set(LIBARCHIVE_LIBRARIES ${_LIBARCHIVE_LIBRARIES})
		set(LIBARCHIVE_INCLUDE_DIRS ${_LIBARCHIVE_INCLUDE_DIRS})
		set(LIBARCHIVE_CFLAGS ${_LIBARCHIVE_CFLAGS_OTHER})
	else()
		set(LIBARCHIVE_LIBRARIES ${_LIBARCHIVE_STATIC_LIBRARIES})
		set(LIBARCHIVE_INCLUDE_DIRS ${_LIBARCHIVE_STATIC_INCLUDE_DIRS})
		set(LIBARCHIVE_CFLAGS ${_LIBARCHIVE_STATIC_CFLAGS_OTHER})
	endif()
else()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_lib_suffix 64)
	else()
		set(_lib_suffix 32)
	endif()

	find_path(LIBARCHIVE_INC
		NAMES archive.h archive_entry.h
		HINTS
			ENV libarchivePath${_lib_suffix}
			ENV libarchivePath
			${_LIBARCHIVE_INCLUDE_DIRS}
		PATHS
			/usr/include /usr/local/include)

	find_library(LIBARCHIVE_LIB
		NAMES archive
		HINTS
			ENV libarchivePath${_lib_suffix}
			ENV libarchivePath
		PATHS
			/usr/lib{_lib_suffix} /usr/local/lib{_lib_suffix}
			/usr/lib /usr/local/lib)

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Libarchive DEFAULT_MSG LIBARCHIVE_LIB LIBARCHIVE_INC)
	mark_as_advanced(LIBARCHIVE_INC LIBARCHIVE_LIB)

	if(LIBARCHIVE_FOUND)
		set(LIBARCHIVE_INCLUDE_DIRS ${LIBARCHIVE_INC})
		set(LIBARCHIVE_LIBRARIES ${LIBARCHIVE_LIB})
		set(LIBARCHIVE_CFLAGS "")
	endif()
endif()