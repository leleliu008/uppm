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

if (DEFINED VCPKG_INSTALLED_DIR)
    set(CMAKE_C_FLAGS              "-L${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib ${CMAKE_C_FLAGS}")
    list(INSERT CMAKE_INCLUDE_PATH 0 "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
    list(INSERT CMAKE_LIBRARY_PATH 0 "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")
endif()

message(STATUS "CMAKE_VERSION        = ${CMAKE_VERSION}")
message(STATUS "CMAKE_PREFIX_PATH    = ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_INCLUDE_PATH   = ${CMAKE_INCLUDE_PATH}")
message(STATUS "CMAKE_LIBRARY_PATH   = ${CMAKE_LIBRARY_PATH}")
message(STATUS "CMAKE_FIND_ROOT_PATH = ${CMAKE_FIND_ROOT_PATH}")

########################################################################################

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(PkgConfig)

# https://cmake.org/cmake/help/latest/module/FindOpenSSL.html
set(OPENSSL_USE_STATIC_LIBS, TRUE)
find_package(OpenSSL REQUIRED)

# https://cmake.org/cmake/help/latest/module/FindCURL.html
find_package(CURL REQUIRED)

# https://cmake.org/cmake/help/latest/module/FindLibArchive.html
find_package(LibArchive REQUIRED)

find_package(LibGit2 REQUIRED)

# https://cmake.org/cmake/help/latest/module/FindLibLZMA.html
find_package(LibLZMA REQUIRED)

# https://cmake.org/cmake/help/latest/module/FindZLIB.html
find_package(ZLIB REQUIRED)

# https://cmake.org/cmake/help/v3.0/module/FindBZip2.html
find_package(BZip2 REQUIRED)

find_package(LibYAML REQUIRED)

find_package(JANSSON REQUIRED)

########################################################################################

aux_source_directory(src/core CORE_SRCS)
aux_source_directory(src           SRCS)

add_executable(uppm "${CORE_SRCS};${SRCS}")

########################################################################################

target_link_libraries(uppm CURL::libcurl)
target_link_libraries(uppm OpenSSL::SSL)
target_link_libraries(uppm OpenSSL::Crypto)
target_link_libraries(uppm LibArchive::LibArchive)
target_link_libraries(uppm LibLZMA::LibLZMA)
target_link_libraries(uppm ${ZLIB_LIBRARIES})
target_link_libraries(uppm ${BZIP2_LIBRARIES})
target_link_libraries(uppm ${LIBGIT2_LIBRARY})
target_link_libraries(uppm ${LIBYAML_LIBRARY})
target_link_libraries(uppm ${JANSSON_LIBRARY})

if (DEFINED VCPKG_INSTALLED_DIR)
    target_link_libraries(uppm -lpcre)
    target_link_libraries(uppm -lhttp_parser)
endif()

include_directories("${CMAKE_BINARY_DIR}")
include_directories("${LIBGIT2_INCLUDE_DIR}")
include_directories("${LIBYAML_INCLUDE_DIR}")
include_directories("${JANSSON_INCLUDE_DIR}")

########################################################################################

configure_file(src/config.h.in config.h @ONLY)

########################################################################################

include(GNUInstallDirs)

install(TARGETS uppm RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(
    FILES       "${CMAKE_CURRENT_SOURCE_DIR}/zsh-completion/_uppm"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/zsh/site-functions"
)
