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

    size_t urlLength = strlen(formula->bin_url);
    size_t urlCopyLength = urlLength + 1;
    char   urlCopy[urlCopyLength];
    memset(urlCopy, 0, urlCopyLength);
    strcpy(urlCopy, formula->bin_url);

    const char * archiveFileName = basename(urlCopy);

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        uppm_formula_free(formula);
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  downloadDirLength = userHomeDirLength + 18;
    char    downloadDir[downloadDirLength];
    memset (downloadDir, 0, downloadDirLength);
    sprintf(downloadDir, "%s/.uppm/downloads", userHomeDir);

    if (!exists_and_is_a_directory(downloadDir)) {
        resultCode = mkdir(downloadDir, S_IRWXU);

        if (resultCode != UPPM_OK) {
            uppm_formula_free(formula);
            return resultCode;
        }
    }

    size_t  archiveFilePathLength = downloadDirLength + strlen(archiveFileName) + 2;
    char    archiveFilePath[archiveFilePathLength];
    memset (archiveFilePath, 0, archiveFilePathLength);
    sprintf(archiveFilePath, "%s/%s", downloadDir, archiveFileName);

    if (exists_and_is_a_regular_file(archiveFilePath)) {
        char * actualSHA256SUM = sha256sum_of_file(archiveFilePath);
        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            free(actualSHA256SUM);
            uppm_formula_free(formula);
            fprintf(stderr, "%s already have been fetched.\n", archiveFilePath);
            return UPPM_OK;
        } else {
            free(actualSHA256SUM);
        }
    }

    resultCode = http_fetch_to_file(formula->bin_url, archiveFilePath, true, true);

    if (resultCode != UPPM_OK) {
        uppm_formula_free(formula);
        return resultCode;
    }

    char * actualSHA256SUM = sha256sum_of_file(archiveFilePath);

    if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
        free(actualSHA256SUM);
        uppm_formula_free(formula);
        return UPPM_OK;
    } else {
        fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", formula->bin_sha, actualSHA256SUM);
        free(actualSHA256SUM);
        uppm_formula_free(formula);
        return UPPM_SHA256_MISMATCH;
    }
}
