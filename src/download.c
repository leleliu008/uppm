#include <time.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/stat.h>

#include "core/tar.h"

#include "sha256sum.h"
#include "uppm.h"

int uppm_download(const char * url, const char * sha256sum, const char * downloadDIR, const char * unpackDIR, size_t stripComponentsNumber, bool verbose) {
    if (url == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (url[0] == '\0') {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (sha256sum == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (sha256sum[0] == '\0') {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (strlen(sha256sum) != 64U) {
        return UPPM_ERROR_ARG_IS_INVALID;
    }

    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////

    size_t   defaultDownloadDIRLength = uppmHomeDIRLength + 11U;
    char     defaultDownloadDIR[defaultDownloadDIRLength];
    snprintf(defaultDownloadDIR, defaultDownloadDIRLength, "%s/downloads", uppmHomeDIR);

    if (downloadDIR == NULL) {
        downloadDIR = defaultDownloadDIR;
    }

    size_t downloadDIRLength = strlen(downloadDIR);

    struct stat st;

    if (lstat(downloadDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            if (unlink(downloadDIR) != 0) {
                perror(downloadDIR);
                return UPPM_ERROR;
            }

            if (mkdir(downloadDIR, S_IRWXU) != 0) {
                if (errno != EEXIST) {
                    return UPPM_ERROR;
                }
            }
        }
    } else {
        if (mkdir(downloadDIR, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                return UPPM_ERROR;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////

    char fileNameExtension[21] = {0};

    ret = uppm_examine_file_extension_from_url(fileNameExtension, 20, url);

    if (ret != UPPM_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////

    size_t   fileNameLength = strlen(fileNameExtension) + 65U;
    char     fileName[fileNameLength];
    snprintf(fileName, fileNameLength, "%s%s", sha256sum, fileNameExtension);

    size_t   filePathLength = downloadDIRLength + fileNameLength + 1U;
    char     filePath[filePathLength];
    snprintf(filePath, filePathLength, "%s/%s", downloadDIR, fileName);

    //////////////////////////////////////////////////////////////////////////

    bool needFetch = true;

    if (lstat(filePath, &st) == 0) {
        if (S_ISREG(st.st_mode)) {
            char actualSHA256SUM[65] = {0};

            if (sha256sum_of_file(actualSHA256SUM, filePath) != 0) {
                return UPPM_ERROR;
            }

            if (strcmp(actualSHA256SUM, sha256sum) == 0) {
                needFetch = false;
            }
        }
    }

    if (needFetch) {
        size_t   tmpStrLength = strlen(url) + 30U;
        char     tmpStr[tmpStrLength];
        snprintf(tmpStr, tmpStrLength, "%s|%ld|%d", url, time(NULL), getpid());

        char tmpFileName[65] = {0};

        ret = sha256sum_of_string(tmpFileName, tmpStr);

        if (ret != 0) {
            return UPPM_ERROR;
        }

        size_t   tmpFilePathLength = downloadDIRLength + 65U;
        char     tmpFilePath[tmpFilePathLength];
        snprintf(tmpFilePath, tmpFilePathLength, "%s/%s", downloadDIR, tmpFileName);

        ret = uppm_http_fetch_to_file(url, tmpFilePath, verbose, verbose);

        if (ret != UPPM_OK) {
            return ret;
        }

        char actualSHA256SUM[65] = {0};

        if (sha256sum_of_file(actualSHA256SUM, tmpFilePath) != 0) {
            return UPPM_ERROR;
        }

        if (strcmp(actualSHA256SUM, sha256sum) == 0) {
            if (rename(tmpFilePath, filePath) == 0) {
                printf("%s\n", filePath);
            } else {
                if (errno == EXDEV) {
                    ret = uppm_copy_file(tmpFilePath, filePath);

                    if (ret != UPPM_OK) {
                        return ret;
                    }
                } else {
                    perror(filePath);
                    return UPPM_ERROR;
                }
            }
        } else {
            fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", sha256sum, actualSHA256SUM);
            return UPPM_ERROR_SHA256_MISMATCH;
        }
    } else {
        fprintf(stderr, "%s already have been fetched.\n", filePath);
    }

    //////////////////////////////////////////////////////////////////////////

    if (unpackDIR != NULL) {
        ret = tar_extract(unpackDIR, filePath, ARCHIVE_EXTRACT_TIME, verbose, stripComponentsNumber);

        if (ret != 0) {
            return abs(ret) + UPPM_ERROR_ARCHIVE_BASE;
        }
    }

    return UPPM_OK;
}
