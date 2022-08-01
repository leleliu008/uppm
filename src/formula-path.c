#include <stdio.h>
#include <string.h>

#include "fs.h"
#include "uppm.h"

int uppm_formula_path(const char * pkgName, char ** out) {
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

    size_t  formulaFilePathLength = userHomeDirLength + 36 + strlen(pkgName);
    char *  formulaFilePath = (char*)calloc(formulaFilePathLength, sizeof(char));
    sprintf(formulaFilePath, "%s/.uppm/repos.d/offical/formula/%s.yml", userHomeDir, pkgName);

    if (exists_and_is_a_regular_file(formulaFilePath)) {
        (*out) = formulaFilePath;
        return UPPM_OK;
    } else {
        free(formulaFilePath);
        formulaFilePath = NULL;
    }

    size_t  reposConfigFilePathLength = userHomeDirLength + 13;
    char    reposConfigFilePath[reposConfigFilePathLength];
    memset (reposConfigFilePath, 0, reposConfigFilePathLength);
    sprintf(reposConfigFilePath, "%s/.uppm/repos", userHomeDir);

    if (!exists_and_is_a_regular_file(reposConfigFilePath)) {
        return UPPM_FORMULA_REPO_NOT_EXIST;
    }

    FILE * reposConfigFile = fopen(reposConfigFilePath, "r");

    if (reposConfigFile == NULL) {
        perror(reposConfigFilePath);
        return UPPM_REPOS_CONFIG_READ_ERROR;
    }

    char line[300];
    while(fgets(line, 300, reposConfigFile)) {
        char * formulaRepoName = strtok(line, "=");

        if (formulaRepoName == NULL) {
            fprintf(stderr, "~/.uppm/repos config file syntax error.\n");
            fclose(reposConfigFile);
            return UPPM_REPOS_CONFIG_READ_ERROR;
        }

        size_t  formulaFilePathLength = userHomeDirLength + 30 + strlen(formulaRepoName) + strlen(pkgName);
        char *  formulaFilePath = (char*)calloc(formulaFilePathLength, sizeof(char));
        sprintf(formulaFilePath, "%s/.uppm/repos.d/%s/formula/%s.yml", userHomeDir, formulaRepoName, pkgName);

        if (exists_and_is_a_regular_file(formulaFilePath)) {
            (*out) = formulaFilePath;
            return UPPM_OK;
        } else {
            free(formulaFilePath);
            formulaFilePath = NULL;
        }
    }

    return UPPM_PACKAGE_IS_NOT_AVAILABLE;
}
