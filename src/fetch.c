#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <sys/stat.h>

#include "core/fs.h"
#include "core/util.h"
#include "core/http.h"
#include "core/sha256sum.h"

#include "uppm.h"

#include <dirent.h>
#include <fnmatch.h>

int uppm_fetch_all_available_packages(bool verbose) {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char *  formulaRepoPath  = formulaRepoList->repos[i]->path;
        size_t  formulaDirLength = strlen(formulaRepoPath) + 10;
        char    formulaDir[formulaDirLength];
        memset (formulaDir, 0, formulaDirLength);
        sprintf(formulaDir, "%s/formula", formulaRepoPath);

        DIR           * dir;
        struct dirent * dir_entry;

        dir = opendir(formulaDir);

        if (dir == NULL) {
            uppm_formula_repo_list_free(formulaRepoList);
            perror(formulaDir);
            return UPPM_ERROR;
        }

        while ((dir_entry = readdir(dir))) {
            //puts(dir_entry->d_name);
            if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
                continue;
            }

            int r = fnmatch("*.yml", dir_entry->d_name, 0);

            if (r == 0) {
                size_t  fileNameLength = strlen(dir_entry->d_name);
                char    packageName[fileNameLength];
                memset (packageName, 0, fileNameLength);
                strncpy(packageName, dir_entry->d_name, fileNameLength - 4);

                //printf("%s\n", packageName);
                resultCode = uppm_fetch(packageName, verbose);

                if (resultCode != UPPM_OK) {
                    uppm_formula_repo_list_free(formulaRepoList);
                    closedir(dir);
                    return resultCode;
                }
            } else if(r == FNM_NOMATCH) {
                ;
            } else {
                uppm_formula_repo_list_free(formulaRepoList);
                fprintf(stderr, "fnmatch() error\n");
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        closedir(dir);
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return UPPM_OK;
}

int uppm_fetch(const char * packageName, bool verbose) {
    if (packageName == NULL) {
        return UPPM_ARG_IS_NULL;
    }

    if (strcmp(packageName, "") == 0) {
        return UPPM_ARG_IS_EMPTY;
    }

    if (strcmp(packageName, "@all") == 0) {
        return uppm_fetch_all_available_packages(verbose);
    }

    UPPMFormula * formula = NULL;

    int resultCode = uppm_formula_parse(packageName, &formula);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        uppm_formula_free(formula);
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        uppm_formula_free(formula);
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  downloadDirLength = userHomeDirLength + 18;
    char    downloadDir[downloadDirLength];
    memset (downloadDir, 0, downloadDirLength);
    sprintf(downloadDir, "%s/.uppm/downloads", userHomeDir);

    if (!exists_and_is_a_directory(downloadDir)) {
        if (mkdir(downloadDir, S_IRWXU) != 0) {
            perror(downloadDir);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }
    }

    char * binFileNameExtension = NULL;

    if (get_file_extension_from_url(&binFileNameExtension, formula->bin_url) < 0) {
        uppm_formula_free(formula);
        return UPPM_ERROR;
    }

    size_t  binFileNameLength = strlen(formula->bin_sha) + strlen(binFileNameExtension) + 1;
    char    binFileName[binFileNameLength];
    memset( binFileName, 0, binFileNameLength);
    sprintf(binFileName, "%s%s", formula->bin_sha, binFileNameExtension);

    free(binFileNameExtension);

    size_t  binFilePathLength = downloadDirLength + binFileNameLength + 1;
    char    binFilePath[binFilePathLength];
    memset (binFilePath, 0, binFilePathLength);
    sprintf(binFilePath, "%s/%s", downloadDir, binFileName);

    if (exists_and_is_a_regular_file(binFilePath)) {
        char * actualSHA256SUM = sha256sum_of_file(binFilePath);
        if (strcmp(actualSHA256SUM, formula->bin_sha) == 0) {
            free(actualSHA256SUM);
            uppm_formula_free(formula);
            fprintf(stderr, "%s already have been fetched.\n", binFilePath);
            return UPPM_OK;
        } else {
            free(actualSHA256SUM);
        }
    }

    if (http_fetch_to_file(formula->bin_url, binFilePath, verbose, verbose) != 0) {
        uppm_formula_free(formula);
        return UPPM_NETWORK_ERROR;
    }

    char * actualSHA256SUM = sha256sum_of_file(binFilePath);

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
