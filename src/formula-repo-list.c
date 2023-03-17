#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_formula_repo_list(UPPMFormulaRepoList * * out) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t uppmFormulaRepoDirLength = userHomeDirLength + 15;
    char   uppmFormulaRepoDir[uppmFormulaRepoDirLength];
    snprintf(uppmFormulaRepoDir, uppmFormulaRepoDirLength, "%s/.uppm/repos.d", userHomeDir);

    struct stat st;

    if ((stat(uppmFormulaRepoDir, &st) != 0) || (!S_ISDIR(st.st_mode))) {
        UPPMFormulaRepoList* formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));

        if (formulaRepoList == NULL) {
            return UPPM_ERROR_MEMORY_ALLOCATE;
        } else {
            (*out) = formulaRepoList;
            return UPPM_OK;
        }
    }

    DIR * dir = opendir(uppmFormulaRepoDir);

    if (dir == NULL) {
        perror(uppmFormulaRepoDir);
        return UPPM_ERROR;
    }

    size_t capcity = 5;

    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = UPPM_OK;

    struct dirent * dir_entry;

    for (;;) {
        errno = 0;

        dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                break;
            } else {
                perror(uppmFormulaRepoDir);
                closedir(dir);
                uppm_formula_repo_list_free(formulaRepoList);
                return UPPM_ERROR;
            }
        }

        //puts(dir_entry->d_name);

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t formulaRepoPathLength = uppmFormulaRepoDirLength + strlen(dir_entry->d_name) + 2;
        char   formulaRepoPath[formulaRepoPathLength];
        snprintf(formulaRepoPath, formulaRepoPathLength, "%s/%s", uppmFormulaRepoDir, dir_entry->d_name);

        size_t formulaRepoConfigFilePathLength = formulaRepoPathLength + 24;
        char   formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
        snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.yml", formulaRepoPath);

        if (stat(formulaRepoConfigFilePath, &st) != 0) {
            continue;
        }

        UPPMFormulaRepo * formulaRepo = NULL;

        ret = uppm_formula_repo_parse(formulaRepoConfigFilePath, &formulaRepo);

        if (ret != UPPM_OK) {
            uppm_formula_repo_free(formulaRepo);
            uppm_formula_repo_list_free(formulaRepoList);
            goto finalize;
        }

        if (formulaRepoList == NULL) {
            formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));

            if (formulaRepoList == NULL) {
                uppm_formula_repo_free(formulaRepo);
                uppm_formula_repo_list_free(formulaRepoList);
                ret = UPPM_ERROR_MEMORY_ALLOCATE;
                goto finalize;
            }

            formulaRepoList->repos = (UPPMFormulaRepo**)calloc(capcity, sizeof(UPPMFormulaRepo*));

            if (formulaRepoList->repos == NULL) {
                uppm_formula_repo_free(formulaRepo);
                uppm_formula_repo_list_free(formulaRepoList);
                ret = UPPM_ERROR_MEMORY_ALLOCATE;
                goto finalize;
            }
        }

        if (capcity == formulaRepoList->size) {
            capcity += 5;
            UPPMFormulaRepo ** formulaRepoArray = (UPPMFormulaRepo**)realloc(formulaRepoList->repos, capcity * sizeof(UPPMFormulaRepo*));

            if (formulaRepoArray == NULL) {
                uppm_formula_repo_free(formulaRepo);
                uppm_formula_repo_list_free(formulaRepoList);
                ret = UPPM_ERROR_MEMORY_ALLOCATE;
                goto finalize;
            } else {
                formulaRepoList->repos = formulaRepoArray;
            }
        }

        formulaRepo->name = strdup(dir_entry->d_name);

        if (formulaRepo->name == NULL) {
            uppm_formula_repo_free(formulaRepo);
            uppm_formula_repo_list_free(formulaRepoList);
            ret = UPPM_ERROR_MEMORY_ALLOCATE;
            goto finalize;
        }

        formulaRepo->path = strdup(formulaRepoPath);

        if (formulaRepo->path == NULL) {
            uppm_formula_repo_free(formulaRepo);
            uppm_formula_repo_list_free(formulaRepoList);
            ret = UPPM_ERROR_MEMORY_ALLOCATE;
            goto finalize;
        }

        formulaRepoList->repos[formulaRepoList->size] = formulaRepo;
        formulaRepoList->size += 1;
    }

    if (formulaRepoList == NULL) {
        formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));

        if (formulaRepoList == NULL) {
            ret = UPPM_ERROR_MEMORY_ALLOCATE;
            goto finalize;
        }
    }

finalize:
    if (ret == UPPM_OK) {
        (*out) = formulaRepoList;
    } else {
        uppm_formula_repo_list_free(formulaRepoList);
    }

    closedir(dir);

    return ret;
}

void uppm_formula_repo_list_free(UPPMFormulaRepoList * formulaRepoList) {
    if (formulaRepoList == NULL) {
        return;
    }

    if (formulaRepoList->repos == NULL) {
        free(formulaRepoList);
        return;
    }

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];
        uppm_formula_repo_free(formulaRepo);
    }

    free(formulaRepoList->repos);
    formulaRepoList->repos = NULL;

    free(formulaRepoList);
}
