#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/zlib-flate.h"
#include "uppm.h"

int uppm_formula_repo_list_new(UPPMFormulaRepoList * * out) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  uppmFormulaRepoDirLength = userHomeDirLength + 15;
    char    uppmFormulaRepoDir[uppmFormulaRepoDirLength];
    memset (uppmFormulaRepoDir, 0, uppmFormulaRepoDirLength);
    snprintf(uppmFormulaRepoDir, uppmFormulaRepoDirLength, "%s/.uppm/repos.d", userHomeDir);

    struct stat st;

    if ((stat(uppmFormulaRepoDir, &st) != 0) || (!S_ISDIR(st.st_mode))) {
        UPPMFormulaRepoList* formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));

        if (formulaRepoList == NULL) {
            return UPPM_ERROR_ALLOCATE_MEMORY_FAILED;
        } else {
            (*out) = formulaRepoList;
            return UPPM_OK;
        }
    }

    DIR           * dir;
    struct dirent * dir_entry;

    dir = opendir(uppmFormulaRepoDir);

    if (dir == NULL) {
        perror(uppmFormulaRepoDir);
        return UPPM_ERROR;
    }

    size_t capcity = 5;

    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = UPPM_OK;

    while ((dir_entry = readdir(dir))) {
        //puts(dir_entry->d_name);
        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t  formulaRepoPathLength = uppmFormulaRepoDirLength + strlen(dir_entry->d_name) + 2;
        char    formulaRepoPath[formulaRepoPathLength];
        memset (formulaRepoPath, 0, formulaRepoPathLength);
        snprintf(formulaRepoPath, formulaRepoPathLength, "%s/%s", uppmFormulaRepoDir, dir_entry->d_name);

        size_t  formulaRepoConfigFilePathLength = formulaRepoPathLength + 24;
        char    formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
        memset (formulaRepoConfigFilePath, 0, formulaRepoConfigFilePathLength);
        snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.dat", formulaRepoPath);

        if ((stat(formulaRepoConfigFilePath, &st) == 0) && S_ISREG(st.st_mode)) {
            UPPMFormulaRepo * formulaRepo = NULL;

            size_t  formulaRepoConfigFilePath2Length = formulaRepoPathLength + 24;
            char    formulaRepoConfigFilePath2[formulaRepoConfigFilePath2Length];
            memset (formulaRepoConfigFilePath2, 0, formulaRepoConfigFilePath2Length);
            snprintf(formulaRepoConfigFilePath2, formulaRepoConfigFilePath2Length, "%s/.uppm-formula-repo.yml", formulaRepoPath);

            FILE * inputFile  = NULL;
            FILE * outputFile = NULL;

            inputFile = fopen(formulaRepoConfigFilePath, "rb");

            if (inputFile == NULL) {
                perror(formulaRepoConfigFilePath);
                goto clean;
            }

            outputFile = fopen(formulaRepoConfigFilePath2, "w");

            if (outputFile == NULL) {
                perror(formulaRepoConfigFilePath2);
                fclose(inputFile);
                goto clean;
            }

            resultCode = zlib_inflate_file_to_file(inputFile, outputFile);

            fclose(inputFile);
            fclose(outputFile);

            if (resultCode != 0) {
                goto clean;
            }

            resultCode = uppm_formula_repo_parse(formulaRepoConfigFilePath2, &formulaRepo);

            if (resultCode != UPPM_OK) {
                //parse failed, treat it as a invalid uppm formula repo.
                continue;
            }

            if (formulaRepoList == NULL) {
                formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));

                if (formulaRepoList == NULL) {
                    uppm_formula_repo_free(formulaRepo);
                    resultCode = UPPM_ERROR_ALLOCATE_MEMORY_FAILED;
                    goto clean;
                }

                formulaRepoList->repos = (UPPMFormulaRepo**)calloc(capcity, sizeof(UPPMFormulaRepo*));

                if (formulaRepoList->repos == NULL) {
                    uppm_formula_repo_free(formulaRepo);
                    resultCode = UPPM_ERROR_ALLOCATE_MEMORY_FAILED;
                    goto clean;
                }
            }

            if (capcity == formulaRepoList->size) {
                capcity += 5;
                UPPMFormulaRepo ** formulaRepoArray = (UPPMFormulaRepo**)realloc(formulaRepoList->repos, capcity * sizeof(UPPMFormulaRepo*));

                if (formulaRepoArray == NULL) {
                    uppm_formula_repo_free(formulaRepo);
                    uppm_formula_repo_list_free(formulaRepoList);
                    resultCode = UPPM_ERROR_ALLOCATE_MEMORY_FAILED;
                    goto clean;
                } else {
                    formulaRepoList->repos = formulaRepoArray;
                }
            }

            formulaRepo->name = strdup(dir_entry->d_name);
            formulaRepo->path = strdup(formulaRepoPath);

            formulaRepoList->repos[formulaRepoList->size] = formulaRepo;
            formulaRepoList->size += 1;
        } else {
            continue;
        }
    }

    if (formulaRepoList == NULL) {
        formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));

        if (formulaRepoList == NULL) {
            resultCode = UPPM_ERROR_ALLOCATE_MEMORY_FAILED;
            goto clean;
        }
    }

clean:
    if (resultCode == UPPM_OK) {
        (*out) = formulaRepoList;
    } else {
        uppm_formula_repo_list_free(formulaRepoList);
    }

    closedir(dir);

    return resultCode;
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
