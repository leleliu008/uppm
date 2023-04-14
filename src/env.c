#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include "core/sysinfo.h"
#include "core/self.h"
#include "uppm.h"

#include <zlib.h>
#include <git2.h>
#include <yaml.h>
#include <jansson.h>
#include <archive.h>
#include <curl/curlver.h>
#include <openssl/opensslv.h>

//#define PCRE2_CODE_UNIT_WIDTH 8
//#include <pcre2.h>

static int uppm_list_dirs(const char * installedDir, size_t installedDirLength, const char * sub) {
    DIR * dir = opendir(installedDir);

    if (dir == NULL) {
        perror(installedDir);
        return UPPM_ERROR;
    }

    struct stat st;

    for (;;) {
        errno = 0;

        struct dirent * dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                break;
            } else {
                perror(installedDir);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        //puts(dir_entry->d_name);

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t packageInstalledDirLength = installedDirLength + strlen(dir_entry->d_name) + 2U;
        char   packageInstalledDir[packageInstalledDirLength];
        snprintf(packageInstalledDir, packageInstalledDirLength, "%s/%s", installedDir, dir_entry->d_name);

        size_t receiptFilePathLength = packageInstalledDirLength + 20U;
        char   receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDir);

        if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            if ((sub == NULL) || (strcmp(sub, "") == 0)) {
                printf("%s\n", packageInstalledDir);
            } else {
                size_t subDirLength = packageInstalledDirLength + strlen(sub) + 2U;
                char   subDir[subDirLength];
                snprintf(subDir, subDirLength, "%s/%s", packageInstalledDir, sub);

                if (stat(subDir, &st) == 0 && S_ISDIR(st.st_mode)) {
                    printf("%s\n", subDir);
                }
            }
        }
    }

    return UPPM_OK;
}

int uppm_env(bool verbose) {
    printf("build.utctime: %s\n\n", UPPM_BUILD_TIMESTAMP);

    //printf("pcre2   : %d.%d\n", PCRE2_MAJOR, PCRE2_MINOR);
    printf("build.libyaml: %s\n", yaml_get_version_string());
    printf("build.libcurl: %s\n", LIBCURL_VERSION);
    printf("build.libgit2: %s\n", LIBGIT2_VERSION);

//https://www.openssl.org/docs/man3.0/man3/OPENSSL_VERSION_BUILD_METADATA.html
//https://www.openssl.org/docs/man1.1.1/man3/OPENSSL_VERSION_TEXT.html
#ifdef OPENSSL_VERSION_STR
    printf("build.openssl: %s\n", OPENSSL_VERSION_STR);
#else
    printf("build.openssl: %s\n", OPENSSL_VERSION_TEXT);
#endif

    printf("build.jansson: %s\n", JANSSON_VERSION);
    printf("build.archive: %s\n", ARCHIVE_VERSION_ONLY_STRING);
    printf("build.zlib:    %s\n\n", ZLIB_VERSION);

    SysInfo sysinfo = {0};

    if (sysinfo_make(&sysinfo) != 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    sysinfo_dump(sysinfo);
    sysinfo_free(sysinfo);

    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t   uppmHomeDirLength = userHomeDirLength + 7U;
    char     uppmHomeDir[uppmHomeDirLength];
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    printf("\n");
    printf("uppm.vers : %s\n", UPPM_VERSION);
    printf("uppm.home : %s\n", uppmHomeDir);

    char * selfRealPath = self_realpath();

    if (selfRealPath == NULL) {
        perror(NULL);
        return UPPM_ERROR;
    }

    printf("uppm.path : %s\n", selfRealPath);

    free(selfRealPath);

    if (!verbose) {
        return UPPM_OK;
    }

    struct stat st;

    size_t   installedDirLength = uppmHomeDirLength + 11U;
    char     installedDir[installedDirLength];
    snprintf(installedDir, installedDirLength, "%s/installed", uppmHomeDir);

    if (stat(installedDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", installedDir);
            return UPPM_ERROR;
        }
    } else {
        return UPPM_OK;
    }

    printf("\nbinDirs:\n");
    uppm_list_dirs(installedDir, installedDirLength, "bin");

    printf("\nlibDirs:\n");
    uppm_list_dirs(installedDir, installedDirLength, "lib");

    printf("\naclocalDirs:\n");
    uppm_list_dirs(installedDir, installedDirLength, "share/aclocal");
    
    return UPPM_OK;
}
