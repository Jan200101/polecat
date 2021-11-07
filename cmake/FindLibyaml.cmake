#
# LIBYAML_INCLUDE_DIRS
# LIBYAML_LIBRARIES
# LIBYAML_CFLAGS

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(_LIBYAML yaml-0.1)

	if (BUILD_STATIC AND NOT _LIBYAML_FOUND)
		 message(FATAL_ERROR "Cannot find static build information")
	endif()
endif()

if (_LIBYAML_FOUND) # we can rely on pkg-config
	if (NOT BUILD_STATIC)
		set(LIBYAML_LIBRARIES ${_LIBYAML_LIBRARIES})
		set(LIBYAML_INCLUDE_DIRS ${_LIBYAML_INCLUDE_DIRS})
		set(LIBYAML_CFLAGS ${_LIBYAML_CFLAGS_OTHER})
	else()
		set(LIBYAML_LIBRARIES ${_LIBYAML_STATIC_LIBRARIES})
		set(LIBYAML_INCLUDE_DIRS ${_LIBYAML_STATIC_INCLUDE_DIRS})
		set(LIBYAML_CFLAGS ${_LIBYAML_STATIC_CFLAGS_OTHER})
	endif()
else()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_lib_suffix 64)
	else()
		set(_lib_suffix 32)
	endif()

	find_path(LIBYAML_INC
		NAMES yaml.h
		HINTS
			ENV libyamlPath${_lib_suffix}
			ENV libyamlPath
			${_LIBYAML_INCLUDE_DIRS}
		PATHS
			/usr/include/)

	find_library(LIBYAML_LIB
		NAMES ${_LIBYAML_LIBRARIES} libyaml
		HINTS
			ENV libyamlPath${_lib_suffix}
			ENV libyamlPath
			${_LIBYAML_LIBRARY_DIRS}
			${_LIBYAML_STATIC_LIBRARY_DIRS}
		PATHS
			/usr/lib{_lib_suffix} /usr/local/lib{_lib_suffix}
			/usr/lib /usr/local/lib)

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Libyaml DEFAULT_MSG LIBYAML_LIB LIBYAML_INC)
	mark_as_advanced(LIBYAML_INC LIBYAML_LIB)

	if(LIBYAML_FOUND)
		set(LIBYAML_INCLUDE_DIRS ${LIBYAML_INC})
		set(LIBYAML_LIBRARIES ${LIBYAML_LIB})
		if (BUILD_STATIC)
			set(LIBYAML_LIBRARIES ${LIBYAML_LIBRARIES} ${_LIBYAML_STATIC_LIBRARIES})
		endif()
	endif()
endif()