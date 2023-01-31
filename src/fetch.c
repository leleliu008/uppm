#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/sha256sum.h"

#include "uppm.h"

int uppm_fetch_all_available_packages(bool verbose) {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret != UPPM_OK) {
        return ret;
    }

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char * formulaRepoPath  = formulaRepoList->repos[i]->path;

        size_t formulaDirLength = strlen(formulaRepoPath) + 10;
        char   formulaDir[formulaDirLength];
        snprintf(formulaDir, formulaDirLength, "%s/formula", formulaRepoPath);

        struct stat status;

        if (stat(formulaDir, &status) != 0) {
            continue;
        }

        if (!S_ISDIR(status.st_mode)) {
            continue;
        }

        DIR           * dir;
        struct dirent * dir_entry;

        dir = opendir(formulaDir);

        if (dir == NULL) {
            perror(formulaDir);
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_ERROR;
        }

        char * fileName;
        char * fileNameSuffix;
        size_t fileNameLength;

        for (;;) {
            errno = 0;

            dir_entry = readdir(dir);

            if (dir_entry == NULL) {
                if (errno == 0) {
                    closedir(dir);
                    break;
                } else {
                    perror(formulaDir);
                    uppm_formula_repo_list_free(formulaRepoList);
                    return UPPM_ERROR;
                }
            }

            fileName = dir_entry->d_name;

            //puts(fileName);

            fileNameLength = strlen(fileName);

            if (fileNameLength > 4) {
                fileNameSuffix = fileName + fileNameLength - 4;

                if (strcmp(fileNameSuffix, ".yml") == 0) {
                    fileName[fileNameLength - 4] = '\0';

                    ret = uppm_fetch(fileName, verbose);

                    if (ret != UPPM_OK) {
                        uppm_formula_repo_list_free(formulaRepoList);
                        closedir(dir);
                        return ret;
                    }
                }
            }
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return UPPM_OK;
}

int uppm_fetch(const char * packageName, bool verbose) {
    if (packageName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (strcmp(packageName, "") == 0) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (strcmp(packageName, "@all") == 0) {
        return uppm_fetch_all_available_packages(verbose);
    }

    UPPMFormula * formula = NULL;

    int ret = uppm_formula_lookup(packageName, &formula);

    if (ret != UPPM_OK) {
        return ret;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        uppm_formula_free(formula);
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        uppm_formula_free(formula);
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    struct stat st;

    size_t downloadDirLength = userHomeDirLength + 18;
    char   downloadDir[downloadDirLength];
    snprintf(downloadDir, downloadDirLength, "%s/.uppm/downloads", userHomeDir);

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

    size_t binFileNameLength = strlen(formula->bin_sha) + strlen(binFileNameExtension) + 1;
    char   binFileName[binFileNameLength];
    snprintf(binFileName, binFileNameLength, "%s%s", formula->bin_sha, binFileNameExtension);

    size_t binFilePathLength = downloadDirLength + binFileNameLength + 1;
    char   binFilePath[binFilePathLength];
    snprintf(binFilePath, binFilePathLength, "%s/%s", downloadDir, binFileName);

    if (stat(binFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        char actualSHA256SUM[65] = {0};

        ret = sha256sum_of_file(actualSHA256SUM, binFilePath);

        if (ret != 0) {
            uppm_formula_free(formula);
            return ret;
        }

        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            uppm_formula_free(formula);
            fprintf(stderr, "%s already have been fetched.\n", binFilePath);
            return UPPM_OK;
        }
    }

    ret = http_fetch_to_file(formula->bin_url, binFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        uppm_formula_free(formula);
        return ret;
    }

    char actualSHA256SUM[65] = {0};

    ret = sha256sum_of_file(actualSHA256SUM, binFilePath);

    if (ret != 0) {
        uppm_formula_free(formula);
        return ret;
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
