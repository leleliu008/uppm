#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>

#include "core/log.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_list_the_available_packages() {
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

    size_t  reposConfigFilePathLength = userHomeDirLength + 13;
    char    reposConfigFilePath[reposConfigFilePathLength];
    memset (reposConfigFilePath, 0, reposConfigFilePathLength);
    sprintf(reposConfigFilePath, "%s/.uppm/repos", userHomeDir);

    if (exists_and_is_a_regular_file(reposConfigFilePath)) {
        FILE * reposConfigFile = fopen(reposConfigFilePath, "r");

        if (reposConfigFile == NULL) {
            perror(reposConfigFilePath);
            return UPPM_REPOS_CONFIG_READ_ERROR;
        }

        char*  formulaRepoNameList[10] = {0};
        size_t formulaRepoNameListSize = 0;

        char line[300];
        while(fgets(line, 300, reposConfigFile)) {
            char * formulaRepoName = strtok(line, "=");

            if (formulaRepoName == NULL) {
                fprintf(stderr, "~/.uppm/repos config file syntax error.\n");
                fclose(reposConfigFile);
                return UPPM_REPOS_CONFIG_READ_ERROR;
            }

            formulaRepoNameList[formulaRepoNameListSize] = formulaRepoName;
            formulaRepoNameListSize++;
        }

        if (formulaRepoNameListSize == 0) {
            fprintf(stderr, "~/.uppm/repos config file no repos.\n");
            fclose(reposConfigFile);
            return UPPM_REPOS_CONFIG_READ_ERROR;
        }

        for (size_t i = 0; i < formulaRepoNameListSize; i++) {
            char *  formulaRepoName = formulaRepoNameList[i];
            size_t  formulaDirLength = userHomeDirLength + strlen(formulaRepoName) + 30;
            char    formulaDir[formulaDirLength];
            memset (formulaDir, 0, formulaDirLength);
            sprintf(formulaDir, "%s/.uppm/repos.d/%s/formula", userHomeDir, formulaRepoName);

            DIR *dir;
            struct dirent *dir_entry;

            dir = opendir(formulaDir);

            if (dir == NULL) {
                perror(formulaDir);
                return UPPM_ERROR;
            } else {
                while ((dir_entry = readdir(dir))) {
                    //puts(dir_entry->d_name);

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
                        fprintf(stderr, "fnmatch() error\n");
                        return UPPM_ERROR;
                    }
                }
                closedir(dir);
            }
        }
    }

    return UPPM_OK;
}
