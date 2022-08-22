#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>

#include "core/log.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_list_the_available_packages() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list_new(&formulaRepoList);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];

        size_t  formulaDirLength = strlen(formulaRepo->path) + 10;
        char    formulaDir[formulaDirLength];
        memset (formulaDir, 0, formulaDirLength);
        sprintf(formulaDir, "%s/formula", formulaRepo->path);

        DIR *dir;
        struct dirent *dir_entry;

        dir = opendir(formulaDir);

        if (dir == NULL) {
            uppm_formula_repo_list_free(formulaRepoList);
            perror(formulaDir);
            return UPPM_ERROR;
        } else {
            while ((dir_entry = readdir(dir))) {
                //puts(dir_entry->d_name);

                if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
                    continue;
                }

                int r = fnmatch("*.yml", dir_entry->d_name, 0);

                if (r == 0) {
                    int fileNameLength = strlen(dir_entry->d_name);
                    char packageName[fileNameLength];
                    memset(packageName, 0, fileNameLength);
                    strncpy(packageName, dir_entry->d_name, fileNameLength - 4);
                    printf("%s\n", packageName);
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
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return UPPM_OK;
}
