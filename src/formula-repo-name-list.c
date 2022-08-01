#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "git.h"
#include "uppm.h"

int uppm_formula_repo_name_list_new(UPPMFormulaRepoNameList * * out) {
    char * userHomeDir = getenv("HOME");
    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  uppmHomeDirLength = userHomeDirLength + 7;
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

    if (!exists_and_is_a_directory(uppmHomeDir)) {
        return UPPM_FORMULA_REPO_NOT_EXIST;
    }

    size_t  formulaRepoConfigFilePathLength = userHomeDirLength + 13;
    char    formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    memset (formulaRepoConfigFilePath, 0, formulaRepoConfigFilePathLength);
    sprintf(formulaRepoConfigFilePath, "%s/.uppm/repos", userHomeDir);

    if (exists_and_is_a_regular_file(formulaRepoConfigFilePath)) {
        FILE * formulaRepoConfigFile = fopen(formulaRepoConfigFilePath, "r");

        if (formulaRepoConfigFile == NULL) {
            perror(formulaRepoConfigFilePath);
            return UPPM_REPOS_CONFIG_READ_ERROR;
        }

        UPPMFormulaRepoNameList * formulaRepoNameList = NULL;

        size_t capacity = 0;

        char line[300];
        while(fgets(line, 300, formulaRepoConfigFile)) {
            char * formulaRepoName = strtok(line, "=");

            if (formulaRepoName == NULL) {
                perror("~/.uppm/repos config file syntax error.");
                fclose(formulaRepoConfigFile);
                uppm_formula_repo_name_list_free(formulaRepoNameList);
                return UPPM_REPOS_CONFIG_READ_ERROR;
            }

            char * formulaRepoPath = (char*)calloc(userHomeDirLength + 16 + strlen(formulaRepoName), sizeof(char));
            sprintf(formulaRepoPath, "%s/.uppm/repos.d/%s", userHomeDir, formulaRepoName);

            if (formulaRepoNameList == NULL) {
                formulaRepoNameList = (UPPMFormulaRepoNameList*)calloc(1, sizeof(UPPMFormulaRepoNameList));
            }

            if (formulaRepoNameList->size == capacity) {
                capacity += 5;
                formulaRepoNameList->repoNames = (char**)realloc(formulaRepoNameList->repoNames, capacity * sizeof(char*));
            }

            formulaRepoNameList->repoNames[formulaRepoNameList->size] = strdup(formulaRepoName);
            formulaRepoNameList->size += 1;
        }

        fclose(formulaRepoConfigFile);

        (*out) = formulaRepoNameList;
    } else {
        UPPMFormulaRepoNameList * formulaRepoNameList = (UPPMFormulaRepoNameList*)calloc(1, sizeof(UPPMFormulaRepoNameList));
        formulaRepoNameList->repoNames = (char**)calloc(1, sizeof(char*));
        formulaRepoNameList->repoNames[0] = strdup("offical");
        formulaRepoNameList->size         = 1;

        (*out) = formulaRepoNameList;
    }

    return UPPM_OK;
}

void uppm_formula_repo_name_list_free(UPPMFormulaRepoNameList * formulaRepoNameList) {
    if (formulaRepoNameList == NULL) {
        return;
    }

    if (formulaRepoNameList->repoNames == NULL) {
        free(formulaRepoNameList);
        return;
    }

    for (size_t i = 0; i < formulaRepoNameList->size; i++) {
        free(formulaRepoNameList->repoNames[i]);
        formulaRepoNameList->repoNames[i] = NULL;
    }

    free(formulaRepoNameList->repoNames);
    formulaRepoNameList->repoNames = NULL;

    free(formulaRepoNameList);
}
