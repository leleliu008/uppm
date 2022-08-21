# try to find yaml.h and libyaml, once done following variables will be defined
#
# LIBYAML_FOUND       - system has libyaml
# LIBYAML_VERSION     - the version of libyaml
# LIBYAML_INCLUDE_DIR - the libyaml include directory
# LIBYAML_LIBRARY     - the filepath of libyaml.a|so|dylib


if (LIBYAML_INCLUDE_DIR AND LIBYAML_LIBRARY)
    set(LIBYAML_FOUND TRUE)
else()
    pkg_check_modules(PKG_CONFIG_LIBYAML QUIET yaml-0.1)

    message("PKG_CONFIG_LIBYAML_FOUND=${PKG_CONFIG_LIBYAML_FOUND}")
    message("PKG_CONFIG_LIBYAML_INCLUDE_DIRS=${PKG_CONFIG_LIBYAML_INCLUDE_DIRS}")
    message("PKG_CONFIG_LIBYAML_LIBRARY_DIRS=${PKG_CONFIG_LIBYAML_LIBRARY_DIRS}")
    message("PKG_CONFIG_LIBYAML_INCLUDEDIR=${PKG_CONFIG_LIBYAML_INCLUDEDIR}")
    message("PKG_CONFIG_LIBYAML_LIBDIR=${PKG_CONFIG_LIBYAML_LIBDIR}")
    message("PKG_CONFIG_LIBYAML_VERSION=${PKG_CONFIG_LIBYAML_VERSION}")
    message("PKG_CONFIG_LIBYAML_LIBRARIES=${PKG_CONFIG_LIBYAML_LIBRARIES}")
    message("PKG_CONFIG_LIBYAML_LINK_LIBRARIES=${PKG_CONFIG_LIBYAML_LINK_LIBRARIES}")

    if (PKG_CONFIG_LIBYAML_FOUND)
        if (PKG_CONFIG_LIBYAML_INCLUDE_DIRS)
            set(LIBYAML_INCLUDE_DIR "${PKG_CONFIG_LIBYAML_INCLUDE_DIRS}")
        elseif (PKG_CONFIG_LIBYAML_INCLUDEDIR)
            set(LIBYAML_INCLUDE_DIR "${PKG_CONFIG_LIBYAML_INCLUDEDIR}")
        else()
            find_path(LIBYAML_INCLUDE_DIR yaml.h)
        endif()

        set(LIBYAML_LIBRARY     "${PKG_CONFIG_LIBYAML_LINK_LIBRARIES}")
    else()
        find_path   (LIBYAML_INCLUDE_DIR yaml.h)
        find_library(LIBYAML_LIBRARY     yaml)
    endif()
    
    if (PKG_CONFIG_LIBYAML_VERSION)
        set(LIBYAML_VERSION ${PKG_CONFIG_LIBYAML_VERSION})
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibYAML REQUIRED_VARS LIBYAML_LIBRARY LIBYAML_INCLUDE_DIR VERSION_VAR LIBYAML_VERSION)

mark_as_advanced(LIBYAML_INCLUDE_DIR LIBYAML_LIBRARY)
