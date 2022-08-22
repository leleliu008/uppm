#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>

#include "core/log.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_search(const char * keyword) {
    if (keyword == NULL) {
        return UPPM_ARG_IS_NULL;
    }

    if (strcmp(keyword, "") == 0) {
        return UPPM_ARG_IS_EMPTY;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode != 0) {
        return resultCode;
    }

    bool isFirst = true;

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        UPPMFormulaRepo* formulaRepo = formulaRepoList->repos[i];

        char * formulaRepoName = formulaRepo->name;

        size_t  formulaDirLength = userHomeDirLength + strlen(formulaRepoName) + 30;
        char    formulaDir[formulaDirLength];
        memset (formulaDir, 0, formulaDirLength);
        sprintf(formulaDir, "%s/.uppm/repos.d/%s/formula", userHomeDir, formulaRepoName);

        DIR * dir;
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

            size_t  patternLength = strlen(keyword) + 7;
            char    pattern[patternLength];
            memset (pattern, 0, patternLength);
            sprintf(pattern, "*%s*.yml", keyword);

            int r = fnmatch(pattern, dir_entry->d_name, 0);

            if (r == 0) {
                size_t  fileNameLength = strlen(dir_entry->d_name);
                char    packageName[fileNameLength];
                memset (packageName, 0, fileNameLength);
                strncpy(packageName, dir_entry->d_name, fileNameLength - 4);

                if (!isFirst) {
                    printf("-------\n");
                }

                //printf("%s\n", packageName);
                resultCode = uppm_info(packageName, NULL);

                if (resultCode != UPPM_OK) {
                    uppm_formula_repo_list_free(formulaRepoList);
                    return resultCode;
                }

                if (isFirst) {
                    isFirst = false;
                }
            } else if(r == FNM_NOMATCH) {
                ;
            } else {
                uppm_formula_repo_list_free(formulaRepoList);
                fprintf(stderr, "fnmatch() error\n");
                return UPPM_ERROR;
            }
        }

        closedir(dir);
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return UPPM_OK;
}
