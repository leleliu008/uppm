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

static int uppm_upgrade_tar_filename(char buf[], const size_t bufSize, const char * latestVersion) {
    char osType[31] = {0};

    if (sysinfo_type(osType, 30) != 0) {
        return UPPM_ERROR;
    }

    char osArch[31] = {0};

    if (sysinfo_arch(osArch, 30) != 0) {
        return UPPM_ERROR;
    }

    char osVers[31] = {0};

    if (sysinfo_vers(osVers, 30) != 0) {
        return UPPM_ERROR;
    }

    int ret;

#if defined (__APPLE__)
    int osVersMajor = 0;

    for (int i = 0; i < 31; i++) {
        if (osVers[i] == '\0') {
            break;
        }

        if (osVers[i] == '.') {
            osVers[i] = '\0';
            osVersMajor = atoi(osVers);
            break;
        }
    }

    if (osVersMajor < 10) {
        fprintf(stderr, "MacOSX %d.x is not supported.\n", osVersMajor);
        return UPPM_ERROR;
    }

    if (osVersMajor > 14) {
        osVersMajor = 14;
    }

    ret = snprintf(buf, bufSize, "uppm-%s-%s-%d.0-%s.tar.xz", latestVersion, osType, osVersMajor, osArch);
#elif defined (__DragonFly__)
    ret = snprintf(buf, bufSize, "uppm-%s-%s-%s-%s.tar.xz", latestVersion, osType, osVers, osArch);
#elif defined (__FreeBSD__)
    double v = atof(osVers) * 10;

    if (v < 130) {
        osVers[0] = '1';
        osVers[1] = '3';
        osVers[2] = '.';
        osVers[3] = '0';
        osVers[4] = '\0';
    } else if (v > 132) {
        osVers[0] = '1';
        osVers[1] = '3';
        osVers[2] = '.';
        osVers[3] = '2';
        osVers[4] = '\0';
    }

    ret = snprintf(buf, bufSize, "uppm-%s-%s-%s-%s.tar.xz", latestVersion, osType, osVers, osArch);
#elif defined (__OpenBSD__)
    double v = atof(osVers) * 10;

    if (v < 73) {
        osVers[0] = '7';
        osVers[1] = '.';
        osVers[2] = '3';
        osVers[3] = '\0';
    } else if (v > 74) {
        osVers[0] = '7';
        osVers[1] = '.';
        osVers[2] = '4';
        osVers[3] = '\0';
    }

    ret = snprintf(buf, bufSize, "uppm-%s-%s-%s-%s.tar.xz", latestVersion, osType, osVers, osArch);
#elif defined (__NetBSD__)
    double v = atof(osVers) * 10;

    if (v < 92) {
        osVers[0] = '9';
        osVers[1] = '.';
        osVers[2] = '2';
        osVers[3] = '\0';
    } else if (v > 93) {
        osVers[0] = '9';
        osVers[1] = '.';
        osVers[2] = '3';
        osVers[3] = '\0';
    }

    ret = snprintf(buf, bufSize, "uppm-%s-%s-%s-%s.tar.xz", latestVersion, osType, osVers, osArch);
#else
    ret = snprintf(buf, bufSize, "uppm-%s-%s-%s.tar.xz", latestVersion, osType, osArch);
#endif

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    } else {
        return UPPM_OK;
    }
}

int uppm_upgrade_self(const bool verbose) {
    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t uppmRunDIRCapacity = uppmHomeDIRLength + 5U;
    char   uppmRunDIR[uppmRunDIRCapacity];

    ret = snprintf(uppmRunDIR, uppmRunDIRCapacity, "%s/run", uppmHomeDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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

    size_t sessionDIRCapacity = uppmRunDIRCapacity + 20U;
    char   sessionDIR[sessionDIRCapacity];

    ret = snprintf(sessionDIR, sessionDIRCapacity, "%s/%d", uppmRunDIR, getpid());

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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

    size_t githubApiResultJsonFilePathCapacity = sessionDIRCapacity + 13U;
    char   githubApiResultJsonFilePath[githubApiResultJsonFilePathCapacity];

    ret = snprintf(githubApiResultJsonFilePath, githubApiResultJsonFilePathCapacity, "%s/latest.json", sessionDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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

    char   latestVersion[11] = {0};
    size_t latestVersionLength = 0U;

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
                        latestVersionLength = n > 10 ? 10 : n;
                        strncpy(latestVersion, p, latestVersionLength);
                    }
                }
            }
        }
    }

finalize:
    fclose(file);

    printf("latestReleaseTagName=%s\n", latestReleaseTagName);

    if (latestReleaseTagName == NULL) {
        fprintf(stderr, "%s return json has no tag_name key.\n", githubApiUrl);
        return UPPM_ERROR;
    }

    if (latestVersion[0] == '\0') {
        fprintf(stderr, "%s return invalid json.\n", githubApiUrl);
        return UPPM_ERROR;
    }

    char    latestVersionCopy[latestVersionLength + 1U];
    strncpy(latestVersionCopy, latestVersion, latestVersionLength + 1U);

    char * latestVersionMajorStr = strtok(latestVersionCopy, ".");
    char * latestVersionMinorStr = strtok(NULL, ".");
    char * latestVersionPatchStr = strtok(NULL, ".");

    int latestVersionMajor = 0;
    int latestVersionMinor = 0;
    int latestVersionPatch = 0;

    if (latestVersionMajorStr != NULL) {
        latestVersionMajor = atoi(latestVersionMajorStr);
    }

    if (latestVersionMinorStr != NULL) {
        latestVersionMinor = atoi(latestVersionMinorStr);
    }

    if (latestVersionPatchStr != NULL) {
        latestVersionPatch = atoi(latestVersionPatchStr);
    }

    if (latestVersionMajor == 0 && latestVersionMinor == 0 && latestVersionPatch == 0) {
        fprintf(stderr, "invalid version format: %s\n", latestVersion);
        return UPPM_ERROR;
    }

    if (latestVersionMajor < UPPM_VERSION_MAJOR) {
        LOG_SUCCESS1("this software is already the latest version.");
        return UPPM_OK;
    } else if (latestVersionMajor == UPPM_VERSION_MAJOR) {
        if (latestVersionMinor < UPPM_VERSION_MINOR) {
            LOG_SUCCESS1("this software is already the latest version.");
            return UPPM_OK;
        } else if (latestVersionMinor == UPPM_VERSION_MINOR) {
            if (latestVersionPatch <= UPPM_VERSION_PATCH) {
                LOG_SUCCESS1("this software is already the latest version.");
                return UPPM_OK;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    size_t tarballFileNameCapacity = 64U;
    char   tarballFileName[tarballFileNameCapacity];

    ret = uppm_upgrade_tar_filename(tarballFileName, tarballFileNameCapacity, latestVersion);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t tarballUrlCapacity = tarballFileNameCapacity + strlen(latestReleaseTagName) + 66U;
    char   tarballUrl[tarballUrlCapacity];

    ret = snprintf(tarballUrl, tarballUrlCapacity, "https://github.com/leleliu008/uppm/releases/download/%s/%s", latestReleaseTagName, tarballFileName);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    size_t tarballFilePathLength = sessionDIRCapacity + tarballFileNameCapacity + 2U;
    char   tarballFilePath[tarballFilePathLength];

    ret = snprintf(tarballFilePath, tarballFilePathLength, "%s/%s", sessionDIR, tarballFileName);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    ret = uppm_http_fetch_to_file(tarballUrl, tarballFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////////////

    ret = tar_extract(sessionDIR, tarballFilePath, 0, verbose, 1);

    if (ret != 0) {
        return abs(ret) + UPPM_ERROR_ARCHIVE_BASE;
    }

    size_t upgradableExecutableFilePathCapacity = sessionDIRCapacity + 10U;
    char   upgradableExecutableFilePath[upgradableExecutableFilePathCapacity];

    ret = snprintf(upgradableExecutableFilePath, upgradableExecutableFilePathCapacity, "%s/bin/uppm", sessionDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

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
        fprintf(stderr, "uppm is up to date with version %s\n", latestVersion);
    } else {
        fprintf(stderr, "Can't upgrade self. the latest version of executable was downloaded to %s, you can manually replace the current running program with it.\n", upgradableExecutableFilePath);
    }

    return ret;
}
