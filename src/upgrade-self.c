#include <string.h>
#include <sys/stat.h>
#include <libgen.h>

#include "core/regex/regex.h"
#include "core/sysinfo.h"
#include "core/untar.h"
#include "core/http.h"
#include "core/util.h"
#include "core/log.h"
#include "uppm.h"

int uppm_upgrade_self(bool verbose) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t  uppmHomeDirLength = userHomeDirLength + 7;
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    if (stat(uppmHomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "not a directory: %s\n", uppmHomeDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t  uppmTmpDirLength = uppmHomeDirLength + 5;
    char    uppmTmpDir[uppmTmpDirLength];
    memset (uppmTmpDir, 0, uppmTmpDirLength);
    snprintf(uppmTmpDir, uppmTmpDirLength, "%s/tmp", uppmHomeDir);

    if (stat(uppmTmpDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "not a directory: %s\n", uppmTmpDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmTmpDir, S_IRWXU) != 0) {
            perror(uppmTmpDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    const char * githubApiUrl = "https://api.github.com/repos/leleliu008/uppm/releases/latest";

    size_t  githubApiResultJsonFilePathLength = uppmTmpDirLength + 13;
    char    githubApiResultJsonFilePath[githubApiResultJsonFilePathLength];
    memset (githubApiResultJsonFilePath, 0, githubApiResultJsonFilePathLength);
    snprintf(githubApiResultJsonFilePath, githubApiResultJsonFilePathLength, "%s/latest.json", uppmTmpDir);

    int resultCode = http_fetch_to_file(githubApiUrl, githubApiResultJsonFilePath, verbose, verbose);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    FILE * file = fopen(githubApiResultJsonFilePath, "r");

    if (file == NULL) {
        perror(githubApiResultJsonFilePath);
        return UPPM_ERROR;
    }

    char * latestVersion = NULL;

    char buf[30];

    size_t j = 0;

    while ((fgets(buf, 30, file)) != NULL) {
        if (regex_matched(buf, "^[[:space:]]*\"tag_name\"")) {
            size_t length = strlen(buf);
            for (size_t i = 10; i < length; i++) {
                if (j == 0) {
                    if (buf[i] >= '0' && buf[i] <= '9') {
                        j = i;
                    }
                } else {
                    if (buf[i] == '"') {
                        buf[i] = '\0';
                        latestVersion = &buf[j];
                        break;
                    }
                }
            }
            break;
        }
    }

    fclose(file);

    if (latestVersion == NULL) {
        fprintf(stderr, "can not get latest version.\n");
        return UPPM_ERROR;
    }

    if (strcmp(latestVersion, UPPM_VERSION) == 0) {
        LOG_SUCCESS("this software is already the latest version.");
        return UPPM_OK;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    char osType[31] = {0};

    if (sysinfo_type(osType, 30) != 0) {
        return UPPM_ERROR;
    }

    char osArch[31] = {0};

    if (sysinfo_arch(osArch, 30) != 0) {
        return UPPM_ERROR;
    }

    size_t  latestVersionLength = strlen(latestVersion);

    size_t  tarballFileNameLength = latestVersionLength + strlen(osType) + strlen(osArch) + 15;
    char    tarballFileName[tarballFileNameLength];
    memset( tarballFileName, 0, tarballFileNameLength);
    snprintf(tarballFileName, tarballFileNameLength, "uppm-%s-%s-%s.tar.xz", latestVersion, osType, osArch);

    size_t  tarballUrlLength = tarballFileNameLength + latestVersionLength + 55;
    char    tarballUrl[tarballUrlLength];
    memset( tarballUrl, 0, tarballUrlLength);
    snprintf(tarballUrl, tarballUrlLength, "https://github.com/leleliu008/uppm/releases/download/%s/%s", latestVersion, tarballFileName);

    size_t  tarballFilePathLength = uppmTmpDirLength + tarballFileNameLength + 2;
    char    tarballFilePath[tarballFilePathLength];
    memset (tarballFilePath, 0, tarballFilePathLength);
    snprintf(tarballFilePath, tarballFilePathLength, "%s/%s", uppmTmpDir, tarballFileName);

    resultCode = http_fetch_to_file(tarballUrl, tarballFilePath, verbose, verbose);

    if ( resultCode != UPPM_OK) {
        return resultCode;
    }

    //////////////////////////////////////////////////////////////////////////////////

    size_t  tarballExtractDirLength = tarballFilePathLength + 3;
    char    tarballExtractDir[tarballExtractDirLength];
    memset (tarballExtractDir, 0, tarballExtractDirLength);
    snprintf(tarballExtractDir, tarballExtractDirLength, "%s.d", tarballFilePath);

    resultCode = untar_extract(tarballExtractDir, tarballFilePath, 0, verbose, 1);

    if (resultCode != 0) {
        return resultCode;
    }

    size_t  upgradableExecutableFilePathLength = tarballExtractDirLength + 10;
    char    upgradableExecutableFilePath[upgradableExecutableFilePathLength];
    memset (upgradableExecutableFilePath, 0, upgradableExecutableFilePathLength);
    snprintf(upgradableExecutableFilePath, upgradableExecutableFilePathLength, "%s/bin/uppm", tarballExtractDir);

    printf("the latest version of executable was downloaded to %s\n", upgradableExecutableFilePath);

    return UPPM_OK;
}
