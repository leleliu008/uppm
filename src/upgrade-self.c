#include <math.h>
#include <errno.h>
#include <string.h>

#include <limits.h>
#include <sys/stat.h>

#include "core/sysinfo.h"
#include "core/self.h"
#include "core/tar.h"
#include "core/log.h"

#include "uppm.h"

int uppm_upgrade_self(bool verbose) {
    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmRunDIRLength = uppmHomeDIRLength + 5U;
    char     uppmRunDIR[uppmRunDIRLength];
    snprintf(uppmRunDIR, uppmRunDIRLength, "%s/run", uppmHomeDIR);

    if (stat(uppmRunDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            if (unlink(uppmRunDIR) == 0) {
                perror(uppmRunDIR);
                return UPPM_ERROR;
            }

            if (mkdir(uppmRunDIR, S_IRWXU) != 0) {
                if (errno != EEXIST) {
                    perror(uppmRunDIR);
                    return UPPM_ERROR;
                }
            }
        }
    } else {
        if (mkdir(uppmRunDIR, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                perror(uppmRunDIR);
                return UPPM_ERROR;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    size_t   sessionDIRLength = uppmRunDIRLength + 20U;
    char     sessionDIR[sessionDIRLength];
    snprintf(sessionDIR, sessionDIRLength, "%s/%d", uppmRunDIR, getpid());

    if (lstat(sessionDIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            ret = uppm_rm_r(sessionDIR, verbose);

            if (ret != UPPM_OK) {
                return ret;
            }

            if (mkdir(sessionDIR, S_IRWXU) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
        } else {
            if (unlink(sessionDIR) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }

            if (mkdir(sessionDIR, S_IRWXU) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
        }
    } else {
        if (mkdir(sessionDIR, S_IRWXU) != 0) {
            perror(sessionDIR);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    const char * githubApiUrl = "https://api.github.com/repos/leleliu008/uppm/releases/latest";

    size_t   githubApiResultJsonFilePathLength = sessionDIRLength + 13U;
    char     githubApiResultJsonFilePath[githubApiResultJsonFilePathLength];
    snprintf(githubApiResultJsonFilePath, githubApiResultJsonFilePathLength, "%s/latest.json", sessionDIR);

    ret = uppm_http_fetch_to_file(githubApiUrl, githubApiResultJsonFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    FILE * file = fopen(githubApiResultJsonFilePath, "r");

    if (file == NULL) {
        perror(githubApiResultJsonFilePath);
        return UPPM_ERROR;
    }

    char * latestReleaseTagName = NULL;
    size_t latestReleaseTagNameLength = 0U;

    char   latestReleaseVersion[11] = {0};
    size_t latestReleaseVersionLength = 0U;

    char * p = NULL;

    char line[70];

    for (;;) {
        p = fgets(line, 70, file);

        if (p == NULL) {
            if (ferror(file)) {
                perror(githubApiResultJsonFilePath);
                goto finalize;
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

            latestReleaseTagName = p;

            size_t n = 0;

            for (;;) {
                if (p[n] == '\0') {
                    fprintf(stderr, "%s return invalid json.\n", githubApiUrl);
                    return UPPM_ERROR;
                }

                if (p[n] == '"') { // found right double quote
                    p[n] = '\0';
                    latestReleaseTagNameLength = n;
                    goto finalize;
                } else {
                    n++;

                    if (p[n] == '+') {
                        latestReleaseVersionLength = n > 10 ? 10 : n;
                        strncpy(latestReleaseVersion, p, latestReleaseVersionLength);
                    }
                }
            }
        }
    }

finalize:
    fclose(file);

    if (latestReleaseTagName == NULL) {
        fprintf(stderr, "%s return json has no tag_name key.\n", githubApiUrl);
        return UPPM_ERROR;
    }

    printf("latestReleaseTagName=%s\n", latestReleaseTagName);

    if (latestReleaseVersion[0] == '\0') {
        fprintf(stderr, "%s return invalid json.\n", githubApiUrl);
        return UPPM_ERROR;
    }

    char    latestReleaseVersionCopy[latestReleaseVersionLength + 1U];
    strncpy(latestReleaseVersionCopy, latestReleaseVersion, latestReleaseVersionLength + 1U);

    char * latestReleaseVersionMajorStr = strtok(latestReleaseVersionCopy, ".");
    char * latestReleaseVersionMinorStr = strtok(NULL, ".");
    char * latestReleaseVersionPatchStr = strtok(NULL, ".");

    int latestReleaseVersionMajor = 0;
    int latestReleaseVersionMinor = 0;
    int latestReleaseVersionPatch = 0;

    if (latestReleaseVersionMajorStr != NULL) {
        latestReleaseVersionMajor = atoi(latestReleaseVersionMajorStr);
    }

    if (latestReleaseVersionMinorStr != NULL) {
        latestReleaseVersionMinor = atoi(latestReleaseVersionMinorStr);
    }

    if (latestReleaseVersionPatchStr != NULL) {
        latestReleaseVersionPatch = atoi(latestReleaseVersionPatchStr);
    }

    if (latestReleaseVersionMajor == 0 && latestReleaseVersionMinor == 0 && latestReleaseVersionPatch == 0) {
        fprintf(stderr, "invalid version format: %s\n", latestReleaseVersion);
        return UPPM_ERROR;
    }

    if (latestReleaseVersionMajor < UPPM_VERSION_MAJOR) {
        LOG_SUCCESS1("this software is already the latest version.");
        return UPPM_OK;
    } else if (latestReleaseVersionMajor == UPPM_VERSION_MAJOR) {
        if (latestReleaseVersionMinor < UPPM_VERSION_MINOR) {
            LOG_SUCCESS1("this software is already the latest version.");
            return UPPM_OK;
        } else if (latestReleaseVersionMinor == UPPM_VERSION_MINOR) {
            if (latestReleaseVersionPatch <= UPPM_VERSION_PATCH) {
                LOG_SUCCESS1("this software is already the latest version.");
                return UPPM_OK;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    char osType[31] = {0};

    if (sysinfo_type(osType, 30) < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    char osArch[31] = {0};

    if (sysinfo_arch(osArch, 30) < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    size_t tarballFileNameLength = strlen(latestReleaseVersion) + strlen(osType) + strlen(osArch) + 15U + 5U;
    char   tarballFileName[tarballFileNameLength];

    if (strcmp(osType, "macos") == 0) {
        char osVersion[31] = {0};

        if (sysinfo_vers(osVersion, 30) != 0) {
            return UPPM_ERROR;
        }

        int i = 0;

        for (;;) {
            char c = osVersion[i];

            if (c == '.') {
                osVersion[i] = '\0';
                break;
            }

            if (c == '\0') {
                break;
            }
        }

        const char * x;

        if (strcmp(osVersion, "10") == 0) {
            x = "10.15";
        } else if (strcmp(osVersion, "11") == 0) {
            x = "11.0";
        } else if (strcmp(osVersion, "12") == 0) {
            x = "12.0";
        } else {
            x = "13.0";
        }

        snprintf(tarballFileName, tarballFileNameLength, "uppm-%s-%s%s-%s.tar.xz", latestReleaseVersion, osType, x, osArch);
    } else {
        snprintf(tarballFileName, tarballFileNameLength, "uppm-%s-%s-%s.tar.xz", latestReleaseVersion, osType, osArch);
    }

    size_t   tarballUrlLength = tarballFileNameLength + strlen(latestReleaseTagName) + 66U;
    char     tarballUrl[tarballUrlLength];
    snprintf(tarballUrl, tarballUrlLength, "https://github.com/leleliu008/uppm/releases/download/%s/%s", latestReleaseTagName, tarballFileName);

    size_t   tarballFilePathLength = sessionDIRLength + tarballFileNameLength + 2U;
    char     tarballFilePath[tarballFilePathLength];
    snprintf(tarballFilePath, tarballFilePathLength, "%s/%s", sessionDIR, tarballFileName);

    ret = uppm_http_fetch_to_file(tarballUrl, tarballFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////////////

    ret = tar_extract(sessionDIR, tarballFilePath, 0, verbose, 1);

    if (ret != 0) {
        return abs(ret) + UPPM_ERROR_ARCHIVE_BASE;
    }

    size_t   upgradableExecutableFilePathLength = sessionDIRLength + 10U;
    char     upgradableExecutableFilePath[upgradableExecutableFilePathLength];
    snprintf(upgradableExecutableFilePath, upgradableExecutableFilePathLength, "%s/bin/uppm", sessionDIR);

    char * selfRealPath = self_realpath();

    if (selfRealPath == NULL) {
        perror(NULL);
        ret = UPPM_ERROR;
        goto finally;
    }

    if (rename(upgradableExecutableFilePath, selfRealPath) != 0) {
        if (errno == EXDEV) {
            if (unlink(selfRealPath) != 0) {
                perror(selfRealPath);
                ret = UPPM_ERROR;
                goto finally;
            }

            ret = uppm_copy_file(upgradableExecutableFilePath, selfRealPath);

            if (ret != UPPM_OK) {
                goto finally;
            }

            if (chmod(selfRealPath, S_IRWXU) != 0) {
                perror(selfRealPath);
                ret = UPPM_ERROR;
            }
        } else {
            perror(selfRealPath);
            ret = UPPM_ERROR;
            goto finally;
        }
    }

finally:
    free(selfRealPath);

    if (ret == UPPM_OK) {
        fprintf(stderr, "uppm is up to date with version %s\n", latestReleaseVersion);
    } else {
        fprintf(stderr, "Can't upgrade self. the latest version of executable was downloaded to %s, you can manually replace the current running program with it.\n", upgradableExecutableFilePath);
    }

    return ret;
}
