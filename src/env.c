#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "core/sysinfo.h"
#include "core/util.h"
#include "core/fs.h"
#include "uppm.h"

#include <git2.h>
#include <yaml.h>
#include <sqlite3.h>
#include <jansson.h>
#include <archive.h>
#include <curl/curlver.h>
#include <openssl/opensslv.h>

//#define PCRE2_CODE_UNIT_WIDTH 8
//#include <pcre2.h>

static int uppm_list_dirs(const char * installedDir, size_t installedDirLength, const char * sub) {
    DIR           * dir;
    struct dirent * dir_entry;

    dir = opendir(installedDir);

    if (dir == NULL) {
        perror(installedDir);
        return UPPM_ERROR;
    }

    while ((dir_entry = readdir(dir))) {
        //puts(dir_entry->d_name);

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t  packageInstalledDirLength = installedDirLength + strlen(dir_entry->d_name) + 2;
        char    packageInstalledDir[packageInstalledDirLength];
        memset (packageInstalledDir, 0, packageInstalledDirLength);
        sprintf(packageInstalledDir, "%s/%s", installedDir, dir_entry->d_name);

        size_t  receiptFilePathLength = packageInstalledDirLength + 20;
        char    receiptFilePath[receiptFilePathLength];
        memset (receiptFilePath, 0, receiptFilePathLength);
        sprintf(receiptFilePath, "%s/.uppm/receipt.yml", packageInstalledDir);

        if (exists_and_is_a_regular_file(receiptFilePath)) {
            if ((sub == NULL) || (strcmp(sub, "") == 0)) {
                puts(packageInstalledDir);
            } else {
                size_t  subDirLength = packageInstalledDirLength + strlen(sub) + 2;
                char    subDir[subDirLength];
                memset (subDir, 0, subDirLength);
                sprintf(subDir, "%s/%s", packageInstalledDir, sub);

                if (exists_and_is_a_directory(subDir)) {
                    puts(subDir);
                }
            }
        }
    }

    closedir(dir);

    return UPPM_OK;
}

int uppm_env(bool verbose) {
    printf("build.time: %s\n\n", UPPM_BUILD_TIMESTAMP);

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

    printf("build.sqlite3: %s\n", SQLITE_VERSION);
    printf("build.jansson: %s\n", JANSSON_VERSION);
    printf("build.archive: %s\n\n", ARCHIVE_VERSION_ONLY_STRING);

    SysInfo sysinfo = {0};

    if (sysinfo_make(&sysinfo) != 0) {
        return UPPM_ERROR;
    }

    sysinfo_dump(sysinfo);
    sysinfo_free(sysinfo);

    printf("\n");

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  uppmHomeDirLength = userHomeDirLength + 7;
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

    printf("uppm.vers : %s\n", UPPM_VERSION);
    printf("uppm.home : %s\n", uppmHomeDir);

    char * path = NULL;

    get_current_executable_realpath(&path);

    printf("uppm.path : %s\n", path == NULL ? "" : path);

    if (path != NULL) {
        free(path);
    }

    if (!verbose) {
        return UPPM_OK;
    }

    size_t  installedDirLength = uppmHomeDirLength + 11;
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    sprintf(installedDir, "%s/installed", uppmHomeDir);

    if (!exists_and_is_a_directory(installedDir)) {
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
