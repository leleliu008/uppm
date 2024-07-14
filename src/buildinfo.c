#include <stdio.h>

#include <zlib.h>
#include <git2.h>
#include <yaml.h>
#include <jansson.h>
#include <archive.h>
#include <curl/curlver.h>
#include <openssl/opensslv.h>

//#define PCRE2_CODE_UNIT_WIDTH 8
//#include <pcre2.h>

#include "uppm.h"

int uppm_buildinfo() {
    printf("uppm.build.utctime: %s\n", UPPM_BUILD_TIMESTAMP);

    //printf("pcre2   : %d.%d\n", PCRE2_MAJOR, PCRE2_MINOR);
    printf("uppm.build.libyaml: %s\n", yaml_get_version_string());
    printf("uppm.build.libcurl: %s\n", LIBCURL_VERSION);
    printf("uppm.build.libgit2: %s\n", LIBGIT2_VERSION);

//https://www.openssl.org/docs/man3.0/man3/OPENSSL_VERSION_BUILD_METADATA.html
//https://www.openssl.org/docs/man1.1.1/man3/OPENSSL_VERSION_TEXT.html
#ifdef OPENSSL_VERSION_STR
    printf("uppm.build.openssl: %s\n", OPENSSL_VERSION_STR);
#else
    printf("uppm.build.openssl: %s\n", OPENSSL_VERSION_TEXT);
#endif

    printf("uppm.build.jansson: %s\n", JANSSON_VERSION);
    printf("uppm.build.archive: %s\n", ARCHIVE_VERSION_ONLY_STRING);
    printf("uppm.build.zlib:    %s\n", ZLIB_VERSION);

    return UPPM_OK;
}
