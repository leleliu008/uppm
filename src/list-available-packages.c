#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_list_the_available_packages(const bool verbose, UPPMPackageNameFilter packageNameFilter, const void * payload) {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t j = 0;

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char * formulaRepoPath  = formulaRepoList->repos[i]->path;

        size_t formulaDIRLength = strlen(formulaRepoPath) + 10U;
        char   formulaDIR[formulaDIRLength];
        snprintf(formulaDIR, formulaDIRLength, "%s/formula", formulaRepoPath);

        struct stat status;

        if (stat(formulaDIR, &status) != 0) {
            continue;
        }

        if (!S_ISDIR(status.st_mode)) {
            continue;
        }

        DIR * dir = opendir(formulaDIR);

        if (dir == NULL) {
            perror(formulaDIR);
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_ERROR;
        }

        char * fileName;
        char * fileNameSuffix;

        for (;;) {
            errno = 0;

            struct dirent * dir_entry = readdir(dir);

            if (dir_entry == NULL) {
                if (errno == 0) {
                    closedir(dir);
                    break;
                } else {
                    perror(formulaDIR);
                    closedir(dir);
                    uppm_formula_repo_list_free(formulaRepoList);
                    return UPPM_ERROR;
                }
            }

            //puts(dir_entry->d_name);

            fileName = dir_entry->d_name;

            size_t fileNameLength = strlen(fileName);

            if (fileNameLength > 4) {
                fileNameSuffix = fileName + fileNameLength - 4;

                if (strcmp(fileNameSuffix, ".yml") == 0) {
                    fileName[fileNameLength - 4] = '\0';

                    ret = packageNameFilter(fileName, j, verbose, payload);

                    j++;

                    if (ret != UPPM_OK) {
                        closedir(dir);
                        uppm_formula_repo_list_free(formulaRepoList);
                        return ret;
                    }
                }
            }
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return UPPM_OK;
}

static size_t j = 0U;

static int package_name_filter(const char * packageName, const size_t i, const bool verbose, const void * payload) {
    if (verbose) {
        if (j != 0U) {
            printf("\n");
        }

        j++;

        return uppm_available_info(packageName, NULL);
    } else {
        printf("%s\n", packageName);
        return UPPM_OK;
    }
}


int uppm_show_the_available_packages(const bool verbose) {
    return uppm_list_the_available_packages(verbose, package_name_filter, NULL);
}
