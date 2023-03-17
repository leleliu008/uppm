#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "core/regex/regex.h"

#include "uppm.h"

int uppm_check_if_the_given_argument_matches_package_name_pattern(const char * arg) {
    if (arg == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    } else if (strcmp(arg, "") == 0) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    } else {
        if (regex_matched(arg, UPPM_PACKAGE_NAME_PATTERN) == 0) {
            return UPPM_OK;
        } else {
            if (errno == 0) {
                return UPPM_ERROR_ARG_IS_INVALID;
            } else {
                perror(NULL);
                return UPPM_ERROR;
            }
        }
    }
}

int uppm_check_if_the_given_package_is_available(const char * packageName) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    UPPMFormulaRepoList * formulaRepoList = NULL;

    ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret != UPPM_OK) {
        uppm_formula_repo_list_free(formulaRepoList);
        return ret;
    }

    struct stat st;

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char * formulaRepoPath = formulaRepoList->repos[i]->path;

        size_t formulaFilePathLength = strlen(formulaRepoPath) + strlen(packageName) + 15;
        char   formulaFilePath[formulaFilePathLength];
        snprintf(formulaFilePath, formulaFilePathLength, "%s/formula/%s.yml", formulaRepoPath, packageName);

        if (stat(formulaFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_OK;
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);
    return UPPM_ERROR_PACKAGE_NOT_AVAILABLE;
}

int uppm_check_if_the_given_package_is_installed(const char * packageName) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    struct stat st;

    size_t uppmHomeDirLength = userHomeDirLength + 7;
    char   uppmHomeDir[uppmHomeDirLength];
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    size_t packageInstalledDirLength = userHomeDirLength + strlen(packageName) + 20;
    char   packageInstalledDir[packageInstalledDirLength];
    snprintf(packageInstalledDir, packageInstalledDirLength, "%s/.uppm/installed/%s", userHomeDir, packageName);

    if (stat(packageInstalledDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", packageInstalledDir);
            return UPPM_ERROR;
        }
    } else {
        return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
    }

    size_t receiptFilePathLength = uppmHomeDirLength + packageInstalledDirLength + 19;
    char   receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDir);

    if (stat(receiptFilePath, &st) == 0) {
        if (S_ISREG(st.st_mode)) {
            return UPPM_OK;
        } else {
            return UPPM_ERROR_PACKAGE_IS_BROKEN;
        }
    } else {
        return UPPM_ERROR_PACKAGE_IS_BROKEN;
    }
}

int uppm_check_if_the_given_package_is_outdated(const char * packageName) {
    UPPMFormula * formula = NULL;
    UPPMReceipt * receipt = NULL;

    int ret = uppm_formula_lookup(packageName, &formula);

    if (ret != UPPM_OK) {
        goto clean;
    }

    ret = uppm_receipt_parse(packageName, &receipt);

    if (ret != UPPM_OK) {
        goto clean;
    }

    if (strcmp(receipt->version, formula->version) == 0) {
        ret = UPPM_ERROR_PACKAGE_NOT_OUTDATED;
    }

clean:
    uppm_formula_free(formula);
    uppm_receipt_free(receipt);
    return ret;
}
