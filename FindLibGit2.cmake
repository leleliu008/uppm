# try to find git2.h and libgit2.a|so|dylib, once done following variables will be defined
#
# LIBGIT2_FOUND         - libgit2.a|so|dylib have been found
# LIBGIT2_VERSION       - the version of libgit2
# LIBGIT2_INCLUDE_DIRS  - the libgit2 and dependencies include directory
# LIBGIT2_LIBRARY_DIRS  - the libgit2 and dependencies lib     directory
# LIBGIT2_LIBRARIES     - the filepath of libgit2.a|so|dylib and it's dependencies


if (LIBGIT2_INCLUDE_DIRS AND LIBGIT2_LIBRARIES)
    set(LIBGIT2_FOUND TRUE)
else()
    pkg_check_modules(PKG_CONFIG_LIBGIT2 QUIET libgit2)

    message("PKG_CONFIG_LIBGIT2_FOUND=${PKG_CONFIG_LIBGIT2_FOUND}")
    message("PKG_CONFIG_LIBGIT2_INCLUDE_DIRS=${PKG_CONFIG_LIBGIT2_INCLUDE_DIRS}")
    message("PKG_CONFIG_LIBGIT2_LIBRARY_DIRS=${PKG_CONFIG_LIBGIT2_LIBRARY_DIRS}")
    message("PKG_CONFIG_LIBGIT2_INCLUDEDIR=${PKG_CONFIG_LIBGIT2_INCLUDEDIR}")
    message("PKG_CONFIG_LIBGIT2_LIBDIR=${PKG_CONFIG_LIBGIT2_LIBDIR}")
    message("PKG_CONFIG_LIBGIT2_VERSION=${PKG_CONFIG_LIBGIT2_VERSION}")
    message("PKG_CONFIG_LIBGIT2_LIBRARIES=${PKG_CONFIG_LIBGIT2_LIBRARIES}")
    message("PKG_CONFIG_LIBGIT2_LINK_LIBRARIES=${PKG_CONFIG_LIBGIT2_LINK_LIBRARIES}")

    if (PKG_CONFIG_LIBGIT2_FOUND)
        set(LIBGIT2_INCLUDE_DIRS ${PKG_CONFIG_LIBGIT2_INCLUDE_DIRS})
        set(LIBGIT2_LIBRARIES     ${PKG_CONFIG_LIBGIT2_LINK_LIBRARIES})
    else()
        find_path   (LIBGIT2_INCLUDE_DIRS git2.h)
        find_library(LIBGIT2_LIBRARIES    git2)
        get_filename_component(LIBGIT2_LIBRARY_DIRS ${LIBGIT2_LIBRARIES} DIRECTORY)
    endif()
    
    if (PKG_CONFIG_LIBGIT2_VERSION)
        set(LIBGIT2_VERSION ${PKG_CONFIG_LIBGIT2_VERSION})
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibGit2 REQUIRED_VARS LIBGIT2_LIBRARIES LIBGIT2_INCLUDE_DIRS VERSION_VAR LIBGIT2_VERSION)

mark_as_advanced(LIBGIT2_INCLUDE_DIRS LIBGIT2_LIBRARIES)
