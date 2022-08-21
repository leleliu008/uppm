# try to find git2.h and libarchive.a|so|dylib, once done following variables will be defined
# https://cmake.org/cmake/help/latest/module/FindLibArchive.html
# LibArchive_FOUND         - libarchive.a|so|dylib have been found
# LibArchive_VERSION       - the version of libarchive
# LibArchive_INCLUDE_DIRS  - the libarchive and dependencies include directory
# LibArchive_LIBRARIES     - the filepath of libarchive.a|so|dylib and it's dependencies


if (LibArchive_INCLUDE_DIRS AND LibArchive_LIBRARIES)
    set(LibArchive_FOUND TRUE)
else()
    pkg_check_modules(PKG_CONFIG_LIBARCHIVE QUIET libarchive)

    message("PKG_CONFIG_LIBARCHIVE_FOUND=${PKG_CONFIG_LIBARCHIVE_FOUND}")
    message("PKG_CONFIG_LIBARCHIVE_INCLUDE_DIRS=${PKG_CONFIG_LIBARCHIVE_INCLUDE_DIRS}")
    message("PKG_CONFIG_LIBARCHIVE_LIBRARY_DIRS=${PKG_CONFIG_LIBARCHIVE_LIBRARY_DIRS}")
    message("PKG_CONFIG_LIBARCHIVE_INCLUDEDIR=${PKG_CONFIG_LIBARCHIVE_INCLUDEDIR}")
    message("PKG_CONFIG_LIBARCHIVE_LIBDIR=${PKG_CONFIG_LIBARCHIVE_LIBDIR}")
    message("PKG_CONFIG_LIBARCHIVE_VERSION=${PKG_CONFIG_LIBARCHIVE_VERSION}")
    message("PKG_CONFIG_LIBARCHIVE_LIBRARIES=${PKG_CONFIG_LIBARCHIVE_LIBRARIES}")
    message("PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES=${PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES}")

    if (PKG_CONFIG_LIBARCHIVE_FOUND)
        if (PKG_CONFIG_LIBARCHIVE_INCLUDE_DIRS)
        	set(LibArchive_INCLUDE_DIRS "${PKG_CONFIG_LIBARCHIVE_INCLUDE_DIRS}")
	    elseif (PKG_CONFIG_LIBARCHIVE_INCLUDEDIR)
        	set(LibArchive_INCLUDE_DIRS "${PKG_CONFIG_LIBARCHIVE_INCLUDEDIR}")
	    else()
		    find_path(LibArchive_INCLUDE_DIRS archive.h)
	    endif()

        # https://sourceware.org/bugzilla/show_bug.cgi?id=21264
        set(PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES2 ${PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES})
        set(PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES )

        foreach(item ${PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES2})
            if ((item MATCHES ".*libm\\.a") OR (item MATCHES ".*libdl\\.a") OR (item MATCHES ".*librt\\.a") OR (item MATCHES ".*libpthread\\.a"))
            else()
                list(APPEND PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES "${item}")
            endif()
        endforeach()

        unset(PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES2)
        #message("----PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES=${PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES}")

        set(LibArchive_LIBRARIES    "${PKG_CONFIG_LIBARCHIVE_LINK_LIBRARIES}")
    else()
        find_path   (LibArchive_INCLUDE_DIRS archive.h)
        find_library(LibArchive_LIBRARIES    archive)
    endif()
    
    if (PKG_CONFIG_LIBARCHIVE_VERSION)
        set(LibArchive_VERSION ${PKG_CONFIG_LIBARCHIVE_VERSION})
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibArchive REQUIRED_VARS LibArchive_LIBRARIES LibArchive_INCLUDE_DIRS VERSION_VAR LibArchive_VERSION)

mark_as_advanced(LibArchive_INCLUDE_DIRS LibArchive_LIBRARIES)
