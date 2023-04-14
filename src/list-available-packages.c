#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_list_the_available_packages(UPPMPackageNameCallbak packageNameCallbak, const void * payload) {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t j = 0;

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char * formulaRepoPath  = formulaRepoList->repos[i]->path;

        size_t formulaDirLength = strlen(formulaRepoPath) + 10U;
        char   formulaDir[formulaDirLength];
        snprintf(formulaDir, formulaDirLength, "%s/formula", formulaRepoPath);

        struct stat status;

        if (stat(formulaDir, &status) != 0) {
            continue;
        }

        if (!S_ISDIR(status.st_mode)) {
            continue;
        }

        DIR * dir = opendir(formulaDir);

        if (dir == NULL) {
            perror(formulaDir);
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_ERROR;
        }

        char * fileName;
        char * fileNameSuffix;
        size_t fileNameLength;

        struct dirent * dir_entry;

        for (;;) {
            errno = 0;

            dir_entry = readdir(dir);

            if (dir_entry == NULL) {
                if (errno == 0) {
                    closedir(dir);
                    break;
                } else {
                    perror(formulaDir);
                    closedir(dir);
                    uppm_formula_repo_list_free(formulaRepoList);
                    return UPPM_ERROR;
                }
            }

            //puts(dir_entry->d_name);

            fileName = dir_entry->d_name;

            fileNameLength = strlen(fileName);

            if (fileNameLength > 4) {
                fileNameSuffix = fileName + fileNameLength - 4;

                if (strcmp(fileNameSuffix, ".yml") == 0) {
                    fileName[fileNameLength - 4] = '\0';

                    ret = packageNameCallbak(fileName, j, payload);

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

static int package_name_callback(const char * packageName, size_t i, const void * payload) {
    printf("%s\n", packageName);
    return UPPM_OK;
}

int uppm_show_the_available_packages() {
    return uppm_list_the_available_packages(package_name_callback, NULL);
}
