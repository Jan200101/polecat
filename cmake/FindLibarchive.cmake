#
# LIBARCHIVE_INCLUDE_DIRS
# LIBARCHIVE_LIBRARIES

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(_LIBARCHIVE QUIET libarchive)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(_lib_suffix 64)
else()
	set(_lib_suffix 32)
endif()

find_path(LIBARCHIVE_INC
	NAMES archive.h archive_entry.h
	HINTS
		ENV libarcguvePath${_lib_suffix}
		ENV libarcguvePath
		${_JSONC_INCLUDE_DIRS}
	PATHS
		/usr/include /usr/local/include)

find_library(LIBARCHIVE_LIB
	NAMES ${_LIBARCHIVE_LIBRARIES} archive
	HINTS
		ENV libarcguvePath${_lib_suffix}
		ENV libarcguvePath
		${_LIBARCHIVE_LIBRARY_DIRS}
	PATHS
		/usr/lib /usr/local/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libarchive DEFAULT_MSG LIBARCHIVE_LIB LIBARCHIVE_INC)
mark_as_advanced(LIBARCHIVE_INC LIBARCHIVE_LIB)

if(LIBARCHIVE_FOUND)
	set(LIBARCHIVE_INCLUDE_DIRS ${LIBARCHIVE_INC})
	set(LIBARCHIVE_LIBRARIES ${LIBARCHIVE_LIB})
endif()