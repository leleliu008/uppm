# https://cmake.org/cmake/help/latest/module/FindCURL.html


if (CURL_INCLUDE_DIRS AND CURL_LIBRARIES)
    set(CURL_FOUND TRUE)
else()
    pkg_check_modules(PKG_CONFIG_CURL QUIET libcurl)

    message("PKG_CONFIG_CURL_FOUND=${PKG_CONFIG_CURL_FOUND}")
    message("PKG_CONFIG_CURL_INCLUDE_DIRS=${PKG_CONFIG_CURL_INCLUDE_DIRS}")
    message("PKG_CONFIG_CURL_LIBRARY_DIRS=${PKG_CONFIG_CURL_LIBRARY_DIRS}")
    message("PKG_CONFIG_CURL_INCLUDEDIR=${PKG_CONFIG_CURL_INCLUDEDIR}")
    message("PKG_CONFIG_CURL_LIBDIR=${PKG_CONFIG_CURL_LIBDIR}")
    message("PKG_CONFIG_CURL_VERSION=${PKG_CONFIG_CURL_VERSION}")
    message("PKG_CONFIG_CURL_LIBRARIES=${PKG_CONFIG_CURL_LIBRARIES}")
    message("PKG_CONFIG_CURL_LINK_LIBRARIES=${PKG_CONFIG_CURL_LINK_LIBRARIES}")

    if (PKG_CONFIG_CURL_FOUND)
        if (PKG_CONFIG_CURL_INCLUDE_DIRS)
        	set(CURL_INCLUDE_DIRS "${PKG_CONFIG_CURL_INCLUDE_DIRS}")
	    elseif (PKG_CONFIG_CURL_INCLUDEDIR)
        	set(CURL_INCLUDE_DIRS "${PKG_CONFIG_CURL_INCLUDEDIR}")
	    else()
		    find_path(CURL_INCLUDE_DIRS curl/curl.h)
	    endif()

        # https://sourceware.org/bugzilla/show_bug.cgi?id=21264

        set(CURL_LIBRARIES )

        foreach(item ${PKG_CONFIG_CURL_LINK_LIBRARIES})
                if(item MATCHES "/.*libm\\.(a|so|dylib)")
            elseif(item MATCHES "/.*libdl\\.(a|so|dylib)")
            elseif(item MATCHES "/.*librt\\.(a|so|dylib)")
            elseif(item MATCHES "/.*libpthread\\.(a|so|dylib)")
            elseif(item MATCHES "/.*\\.framework")
                get_filename_component(LIBRARIE_FILENAME ${item} NAME_WLE)
                message(STATUS "${LIBRARIE_FILENAME}")
                list(APPEND CURL_LIBRARIES "-framework ${LIBRARIE_FILENAME}")
            elseif(item MATCHES "/.*")
                get_filename_component(LIBRARIE_FILENAME ${item} NAME)

                message(STATUS "${LIBRARIE_FILENAME}")

                if (NOT TARGET  CURL::${LIBRARIE_FILENAME})
                    add_library(CURL::${LIBRARIE_FILENAME} UNKNOWN IMPORTED)
                    set_target_properties(CURL::${LIBRARIE_FILENAME} PROPERTIES IMPORTED_LOCATION "${item}")
                endif()

                list(APPEND CURL_LIBRARIES CURL::${LIBRARIE_FILENAME})
            endif()
        endforeach()

        if (NOT TARGET  CURL::libcurl)
            add_library(CURL::libcurl INTERFACE IMPORTED)
            set_target_properties(CURL::libcurl PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES      "${CURL_LIBRARIES}"
            )
        endif()

        if (PKG_CONFIG_CURL_VERSION)
            set(CURL_VERSION_STRING ${PKG_CONFIG_CURL_VERSION})
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CURL REQUIRED_VARS CURL_LIBRARIES CURL_INCLUDE_DIRS VERSION_VAR CURL_VERSION)

mark_as_advanced(CURL_INCLUDE_DIRS CURL_LIBRARIES)
