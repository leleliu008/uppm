#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_search(const char * keyword) {
    if (keyword == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t keywordLength = strlen(keyword);

    if (keywordLength == 0) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    bool isFirst = true;

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char *  formulaRepoPath  = formulaRepoList->repos[i]->path;

        size_t  formulaDirLength = strlen(formulaRepoPath) + 10;
        char    formulaDir[formulaDirLength];
        memset (formulaDir, 0, formulaDirLength);
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

        while ((dir_entry = readdir(dir))) {
            //puts(dir_entry->d_name);
            if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
                continue;
            }

            size_t  patternLength = keywordLength + 7;
            char    pattern[patternLength];
            memset (pattern, 0, patternLength);
            snprintf(pattern, patternLength, "*%s*.yml", keyword);

            int r = fnmatch(pattern, dir_entry->d_name, 0);

            if (r == 0) {
                size_t  fileNameLength = strlen(dir_entry->d_name);
                char    packageName[fileNameLength];
                memset (packageName, 0, fileNameLength);
                strncpy(packageName, dir_entry->d_name, fileNameLength - 4);

                if (isFirst) {
                    isFirst = false;
                } else {
                    printf("\n");
                }

                //printf("%s\n", packageName);
                resultCode = uppm_info(packageName, NULL);

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
