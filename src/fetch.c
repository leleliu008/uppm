#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/sha256sum.h"

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

    UPPMFormula * formula = NULL;

    int ret = uppm_formula_lookup(packageName, &formula);

    if (ret != UPPM_OK) {
        return ret;
    }

    char   uppmHomeDir[256];
    size_t uppmHomeDirLength;

    ret = uppm_home_dir(uppmHomeDir, 256, &uppmHomeDirLength);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return ret;
    }

    struct stat st;

    size_t   downloadDirLength = uppmHomeDirLength + 11U;
    char     downloadDir[downloadDirLength];
    snprintf(downloadDir, downloadDirLength, "%s/downloads", uppmHomeDir);

    if (stat(downloadDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", downloadDir);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(downloadDir, S_IRWXU) != 0) {
            perror(downloadDir);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    }

    char binFileNameExtension[21] = {0};

    ret = uppm_examine_file_extension_from_url(binFileNameExtension, 20, formula->bin_url);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return ret;
    }

    size_t   binFileNameLength = strlen(formula->bin_sha) + strlen(binFileNameExtension) + 1U;
    char     binFileName[binFileNameLength];
    snprintf(binFileName, binFileNameLength, "%s%s", formula->bin_sha, binFileNameExtension);

    size_t   binFilePathLength = downloadDirLength + binFileNameLength + 1U;
    char     binFilePath[binFilePathLength];
    snprintf(binFilePath, binFilePathLength, "%s/%s", downloadDir, binFileName);

    if (stat(binFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        char actualSHA256SUM[65] = {0};

        if (sha256sum_of_file(actualSHA256SUM, binFilePath) != 0) {
            perror(binFilePath);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            uppm_formula_free(formula);
            fprintf(stderr, "%s already have been fetched.\n", binFilePath);
            return UPPM_OK;
        }
    }

    ret = uppm_http_fetch_to_file(formula->bin_url, binFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return ret;
    }

    char actualSHA256SUM[65] = {0};

    if (sha256sum_of_file(actualSHA256SUM, binFilePath) != 0) {
        perror(binFilePath);
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
        uppm_formula_free(formula);
        printf("%s\n", binFilePath);
        return UPPM_OK;
    } else {
        uppm_formula_free(formula);
        fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", formula->bin_sha, actualSHA256SUM);
        return UPPM_ERROR_SHA256_MISMATCH;
    }
}
