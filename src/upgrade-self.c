#include <math.h>
#include <string.h>
#include <sys/stat.h>

#include "core/sysinfo.h"
#include "core/untar.h"
#include "core/http.h"
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

    size_t uppmHomeDirLength = userHomeDirLength + 7;
    char   uppmHomeDir[uppmHomeDirLength];
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    if (stat(uppmHomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmHomeDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t uppmTmpDirLength = uppmHomeDirLength + 5;
    char   uppmTmpDir[uppmTmpDirLength];
    snprintf(uppmTmpDir, uppmTmpDirLength, "%s/tmp", uppmHomeDir);

    if (stat(uppmTmpDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmTmpDir);
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

    size_t githubApiResultJsonFilePathLength = uppmTmpDirLength + 13;
    char   githubApiResultJsonFilePath[githubApiResultJsonFilePathLength];
    snprintf(githubApiResultJsonFilePath, githubApiResultJsonFilePathLength, "%s/latest.json", uppmTmpDir);

    int ret = http_fetch_to_file(githubApiUrl, githubApiResultJsonFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    FILE * file = fopen(githubApiResultJsonFilePath, "r");

    if (file == NULL) {
        perror(githubApiResultJsonFilePath);
        return UPPM_ERROR;
    }

    char * latestVersion = NULL;

    char * p = NULL;

    char line[30];

    for (;;) {
        p = fgets(line, 30, file);

        if (p == NULL) {
            if (ferror(file)) {
                perror(githubApiResultJsonFilePath);
                goto finally;
            } else {
                break;
            }
        }

        for (;;) {
            if (p[0] <= 32) { // non-printable ASCII characters and space
                p++;
            } else {
                break;
            }
        }

        if (strncmp(p, "\"tag_name\"", 10) == 0) {
            p += 10;

            for (;;) {
                if (p[0] == '\0') {
                    fprintf(stderr, "%s return invalid json.\n", githubApiUrl);
                    return UPPM_ERROR;
                }

                if (p[0] == '"') { // found left double quote
                    p++;
                    break;
                } else {
                    p++;
                }
            }

            size_t n = 0;
            char * q = p;

            for (;;) {
                if (q[n] == '\0') {
                    fprintf(stderr, "%s return invalid json.\n", githubApiUrl);
                    return UPPM_ERROR;
                }

                if (q[n] == '"') { // found right double quote
                    q[n] = '\0';
                    latestVersion = &q[0];
                    goto finally;
                } else {
                    n++;
                }
            }
        }
    }

finally:
    fclose(file);

    printf("latestVersion=%s\n", latestVersion);

    if (latestVersion == NULL) {
        fprintf(stderr, "%s return json has no tag_name key.\n", githubApiUrl);
        return UPPM_ERROR;
    }

    if (strcmp(latestVersion, UPPM_VERSION) == 0) {
        LOG_SUCCESS("this software is already the latest version.");
        return UPPM_OK;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    char osType[31] = {0};

    ret = sysinfo_type(osType, 30);

    if (ret != UPPM_OK) {
        return ret;
    }

    char osArch[31] = {0};

    ret = sysinfo_arch(osArch, 30);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t latestVersionLength = strlen(latestVersion);

    size_t tarballFileNameLength = latestVersionLength + strlen(osType) + strlen(osArch) + 15;
    char   tarballFileName[tarballFileNameLength];
    snprintf(tarballFileName, tarballFileNameLength, "uppm-%s-%s-%s.tar.xz", latestVersion, osType, osArch);

    size_t tarballUrlLength = tarballFileNameLength + latestVersionLength + 55;
    char   tarballUrl[tarballUrlLength];
    snprintf(tarballUrl, tarballUrlLength, "https://github.com/leleliu008/uppm/releases/download/%s/%s", latestVersion, tarballFileName);

    size_t tarballFilePathLength = uppmTmpDirLength + tarballFileNameLength + 2;
    char   tarballFilePath[tarballFilePathLength];
    snprintf(tarballFilePath, tarballFilePathLength, "%s/%s", uppmTmpDir, tarballFileName);

    ret = http_fetch_to_file(tarballUrl, tarballFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////////////

    size_t tarballExtractDirLength = tarballFilePathLength + 3;
    char   tarballExtractDir[tarballExtractDirLength];
    snprintf(tarballExtractDir, tarballExtractDirLength, "%s.d", tarballFilePath);

    ret = untar_extract(tarballExtractDir, tarballFilePath, 0, verbose, 1);

    if (ret != 0) {
        return abs(ret) + UPPM_ERROR_ARCHIVE_BASE;
    }

    size_t upgradableExecutableFilePathLength = tarballExtractDirLength + 10;
    char   upgradableExecutableFilePath[upgradableExecutableFilePathLength];
    snprintf(upgradableExecutableFilePath, upgradableExecutableFilePathLength, "%s/bin/uppm", tarballExtractDir);

    printf("the latest version of executable was downloaded to %s\n", upgradableExecutableFilePath);

    return UPPM_OK;
}
