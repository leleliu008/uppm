cmake_minimum_required(VERSION 3.0)

project(uppm C)

set(PROJECT_VERSION       1.0.0)
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

########################################################################################

option(BUILD_STATICALLY_LINKED_EXECUTABLE "if build statically linked executable" OFF)

########################################################################################

if (DEFINED VCPKG_INSTALLED_DIR)
    set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/pkgconfig")
    set(CMAKE_C_FLAGS              "-L${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib ${CMAKE_C_FLAGS}")
    list(INSERT CMAKE_INCLUDE_PATH 0 "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
    list(INSERT CMAKE_LIBRARY_PATH 0 "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")
endif()

message(STATUS "CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_NAME      = ${CMAKE_SYSTEM_NAME}")

message(STATUS "CMAKE_VERSION        = ${CMAKE_VERSION}")
message(STATUS "CMAKE_PREFIX_PATH    = ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_INCLUDE_PATH   = ${CMAKE_INCLUDE_PATH}")
message(STATUS "CMAKE_LIBRARY_PATH   = ${CMAKE_LIBRARY_PATH}")
message(STATUS "CMAKE_FIND_ROOT_PATH = ${CMAKE_FIND_ROOT_PATH}")

########################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

if (BUILD_STATICALLY_LINKED_EXECUTABLE)
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL Darwin)
        message(WARNING "macOS DO NOT support static link the c runtime.")
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif()
endif()

find_package(PkgConfig)

# https://cmake.org/cmake/help/latest/module/FindOpenSSL.html
find_package(OpenSSL REQUIRED)

# https://cmake.org/cmake/help/latest/module/FindCURL.html
find_package(CURL REQUIRED)

# https://cmake.org/cmake/help/latest/module/FindLibArchive.html
find_package(LibArchive REQUIRED)

find_package(LibGit2 REQUIRED)

find_package(LibYAML REQUIRED)

find_package(JANSSON REQUIRED)

########################################################################################

if (OPENSSL_VERSION)
    message(STATUS "OPENSSL_VERSION = ${OPENSSL_VERSION}")

    string(REPLACE "." ";" OPENSSL_VERSION_PART_LIST ${OPENSSL_VERSION})
    list(GET OPENSSL_VERSION_PART_LIST 0 OPENSSL_VERSION_MAJOR)

    message(STATUS "OPENSSL_VERSION_MAJOR = ${OPENSSL_VERSION_MAJOR}")
else()
    message(FATAL "no OPENSSL_VERSION set.")
endif()

aux_source_directory(src/core CORE_SRCS)
aux_source_directory(src           SRCS)

add_executable(uppm "${CORE_SRCS};${SRCS}")

########################################################################################

message(STATUS "CURL_LIBRARIES=${CURL_LIBRARIES}")
message(STATUS "OPENSSL_LIBRARIES=${OPENSSL_LIBRARIES}")
message(STATUS "LibArchive_LIBRARIES=${LibArchive_LIBRARIES}")
message(STATUS "LIBGIT2_LIBRARIES=${LIBGIT2_LIBRARIES}")
message(STATUS "LIBYAML_LIBRARY=${LIBYAML_LIBRARY}")
message(STATUS "JANSSON_LIBRARY=${JANSSON_LIBRARY}")

target_link_libraries(uppm CURL::libcurl)
target_link_libraries(uppm OpenSSL::SSL)
target_link_libraries(uppm OpenSSL::Crypto)
#target_link_libraries(uppm "${LibArchive_LIBRARIES}")
#target_link_libraries(uppm "${LIBGIT2_LIBRARIES}")
#target_link_libraries(uppm "${LIBYAML_LIBRARY}")
#target_link_libraries(uppm "${JANSSON_LIBRARY}")
target_link_libraries(uppm LibArchive::LibArchive)
target_link_libraries(uppm LIBGIT2::LIBGIT2)
target_link_libraries(uppm LIBYAML::LIBYAML)
target_link_libraries(uppm JANSSON::JANSSON)

if (BUILD_STATICALLY_LINKED_EXECUTABLE)
    target_link_options(uppm PRIVATE -static)
endif()

include_directories("${CMAKE_BINARY_DIR}")
#include_directories("${LibArchive_INCLUDE_DIRS}")
#include_directories("${LIBGIT2_INCLUDE_DIRS}")
#include_directories("${LIBYAML_INCLUDE_DIR}")
#include_directories("${JANSSON_INCLUDE_DIR}")

########################################################################################

configure_file(src/config.h.in config.h @ONLY)

########################################################################################

include(GNUInstallDirs)

install(TARGETS uppm RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(
    FILES       "${CMAKE_CURRENT_SOURCE_DIR}/zsh-completion/_uppm"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/zsh/site-functions"
)
