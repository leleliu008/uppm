#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <sys/stat.h>

#include "core/fs.h"
#include "core/http.h"
#include "core/sha256sum.h"

#include "uppm.h"

int uppm_fetch(const char * packageName) {
    UPPMFormula * formula = NULL;

    int resultCode = uppm_formula_parse(packageName, &formula);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    size_t  urlLength = strlen(formula->bin_url);
    size_t  urlCopyLength = urlLength + 1;
    char    urlCopy[urlCopyLength];
    memset (urlCopy, 0, urlCopyLength);
    strcpy(urlCopy, formula->bin_url);

    const char * binFileName = basename(urlCopy);

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        free(formula);
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  binDirLength = userHomeDirLength + 18;
    char    binDir[binDirLength];
    memset (binDir, 0, binDirLength);
    sprintf(binDir, "%s/.uppm/downloads", userHomeDir);

    if (!exists_and_is_a_directory(binDir)) {
        resultCode = mkdir(binDir, S_IRWXU);

        if (resultCode != UPPM_OK) {
            free(formula);
            return resultCode;
        }
    }

    size_t  binFilePathLength = binDirLength + strlen(binFileName) + 2;
    char    binFilePath[binFilePathLength];
    memset (binFilePath, 0, binFilePathLength);
    sprintf(binFilePath, "%s/%s", binDir, binFileName);

    if (exists_and_is_a_regular_file(binFilePath)) {
        char * actualSHA256SUM = sha256sum_of_file(binFilePath);
        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            free(actualSHA256SUM);
            free(formula);
            fprintf(stderr, "%s already have been fetched.\n", binFilePath);
            return UPPM_OK;
        } else {
            free(actualSHA256SUM);
        }
    }

    resultCode = http_fetch_to_file(formula->bin_url, binFilePath, true, true);

    if (resultCode != UPPM_OK) {
        free(formula);
        return resultCode;
    }

    char * actualSHA256SUM = sha256sum_of_file(binFilePath);

    if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
        free(actualSHA256SUM);
        free(formula);
        return UPPM_OK;
    } else {
        fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", formula->bin_sha, actualSHA256SUM);
        free(actualSHA256SUM);
        free(formula);
        return UPPM_SHA256_MISMATCH;
    }
}
