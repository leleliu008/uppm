# try to find libjansson, once done following variables will be defined
#
# JANSSON_FOUND       - system has libjansson
# JANSSON_VERSION     - the version of libansson
# JANSSON_INCLUDE_DIR - the libjansson include directory
# JANSSON_LIBRARY     - the filepath of libjansson.a|so|dylib


if (JANSSON_INCLUDE_DIR AND JANSSON_LIBRARY)
    set(JANSSON_FOUND TRUE)
else()
    pkg_check_modules(PKG_CONFIG_JANSSON QUIET jansson)

    #message("PKG_CONFIG_JANSSON_FOUND=${PKG_CONFIG_JANSSON_FOUND}")
    #message("PKG_CONFIG_JANSSON_INCLUDE_DIRS=${PKG_CONFIG_JANSSON_INCLUDE_DIRS}")
    #message("PKG_CONFIG_JANSSON_LIBRARY_DIRS=${PKG_CONFIG_JANSSON_LIBRARY_DIRS}")
    #message("PKG_CONFIG_JANSSON_INCLUDEDIR=${PKG_CONFIG_JANSSON_INCLUDEDIR}")
    #message("PKG_CONFIG_JANSSON_LIBDIR=${PKG_CONFIG_JANSSON_LIBDIR}")
    #message("PKG_CONFIG_JANSSON_VERSION=${PKG_CONFIG_JANSSON_VERSION}")

    if (ENABLE_STATIC)
        set(JANSSON_LIBRARY_NAMES libjansson.a)
    else()
        set(JANSSON_LIBRARY_NAMES jansson)
    endif()

    if (PKG_CONFIG_JANSSON_FOUND)
        find_path(JANSSON_INCLUDE_DIR jansson.h                HINTS ${PKG_CONFIG_JANSSON_INCLUDE_DIRS})
        find_library(JANSSON_LIBRARY  ${JANSSON_LIBRARY_NAMES} HINTS ${PKG_CONFIG_JANSSON_LIBRARY_DIRS})
    else()
        find_path(JANSSON_INCLUDE_DIR jansson.h)
        find_library(JANSSON_LIBRARY  ${JANSSON_LIBRARY_NAMES})
    endif()
    
    if (JANSSON_INCLUDE_DIR)
        if (PKG_CONFIG_JANSSON_VERSION)
            set(JANSSON_VERSION ${PKG_CONFIG_JANSSON_VERSION})
        else()
	        file(STRINGS "${JANSSON_INCLUDE_DIR}/jansson.h" JANSSON_VERSION REGEX "^#define[\t ]+JANSSON_VERSION[\t ]+\".*\"")
	        string(REGEX REPLACE "^#define[\t ]+JANSSON_VERSION[\t ]+\"([^\"]*)\".*" "\\1" JANSSON_VERSION "${JANSSON_VERSION}")
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JANSSON REQUIRED_VARS JANSSON_LIBRARY JANSSON_INCLUDE_DIR VERSION_VAR JANSSON_VERSION)

mark_as_advanced(JANSSON_INCLUDE_DIR JANSSON_LIBRARY)
