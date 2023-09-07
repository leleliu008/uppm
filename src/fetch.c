#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>

#include "core/http.h"
#include "sha256sum.h"

#include "uppm.h"

static int package_name_callback(const char * packageName, size_t i, const void * payload) {
    return uppm_fetch(packageName, *((bool*)payload));
}

int uppm_fetch(const char * packageName, bool verbose) {
    if (packageName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (packageName[0] == '\0') {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (strcmp(packageName, "@all") == 0) {
        return uppm_list_the_available_packages(package_name_callback, &verbose);
    }

    //////////////////////////////////////////////////////////////////////////

    UPPMFormula * formula = NULL;

    int ret = uppm_formula_lookup(packageName, &formula);

    if (ret != UPPM_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////

    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   downloadDIRLength = uppmHomeDIRLength + 11U;
    char     downloadDIR[downloadDIRLength];
    snprintf(downloadDIR, downloadDIRLength, "%s/downloads", uppmHomeDIR);

    if (stat(downloadDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", downloadDIR);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(downloadDIR, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                perror(downloadDIR);
                uppm_formula_free(formula);
                return UPPM_ERROR;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////

    char fileNameExtension[21] = {0};

    ret = uppm_examine_file_extension_from_url(fileNameExtension, 20, formula->bin_url);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////

    size_t   fileNameLength = strlen(formula->bin_sha) + strlen(fileNameExtension) + 1U;
    char     fileName[fileNameLength];
    snprintf(fileName, fileNameLength, "%s%s", formula->bin_sha, fileNameExtension);

    size_t   filePathLength = downloadDIRLength + fileNameLength + 1U;
    char     filePath[filePathLength];
    snprintf(filePath, filePathLength, "%s/%s", downloadDIR, fileName);

    //////////////////////////////////////////////////////////////////////////

    if (lstat(filePath, &st) == 0 && S_ISREG(st.st_mode)) {
        char actualSHA256SUM[65] = {0};

        if (sha256sum_of_file(actualSHA256SUM, filePath) != 0) {
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            fprintf(stdout, "%s\n", filePath);
            uppm_formula_free(formula);
            return UPPM_OK;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    size_t   tmpStrLength = strlen(formula->bin_url) + 30U;
    char     tmpStr[tmpStrLength];
    snprintf(tmpStr, tmpStrLength, "%s|%ld|%d", formula->bin_url, time(NULL), getpid());

    char tmpFileName[65] = {0};

    ret = sha256sum_of_string(tmpFileName, tmpStr);

    if (ret != 0) {
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    size_t   tmpFilePathLength = downloadDIRLength + 66U;
    char     tmpFilePath[tmpFilePathLength];
    snprintf(tmpFilePath, tmpFilePathLength, "%s/%s", downloadDIR, tmpFileName);

    //////////////////////////////////////////////////////////////////////////

    ret = uppm_http_fetch_to_file(formula->bin_url, tmpFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return ret;
    }

    char actualSHA256SUM[65] = {0};

    if (sha256sum_of_file(actualSHA256SUM, tmpFilePath) != 0) {
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    if (strcmp(actualSHA256SUM, formula->bin_sha) != 0) {
        uppm_formula_free(formula);
        fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", formula->bin_sha, actualSHA256SUM);
        return UPPM_ERROR_SHA256_MISMATCH;
    }

    if (chdir(downloadDIR) != 0) {
        perror(downloadDIR);
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    if (rename(tmpFileName, fileName) == 0) {
        uppm_formula_free(formula);
        printf("%s\n", filePath);
        return UPPM_OK;
    } else {
        perror(fileName);
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }
}
