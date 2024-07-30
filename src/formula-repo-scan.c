#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_formula_repo_scan(UPPMFormulaRepoCallback callback, const void * payload) {
    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t uppmFormulaRepoDIRCapacity = uppmHomeDIRLength + 9U;
    char   uppmFormulaRepoDIR[uppmFormulaRepoDIRCapacity];

    ret = snprintf(uppmFormulaRepoDIR, uppmFormulaRepoDIRCapacity, "%s/repos.d", uppmHomeDIR);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    struct stat st;

    if ((stat(uppmFormulaRepoDIR, &st) != 0) || (!S_ISDIR(st.st_mode))) {
        return UPPM_OK;
    }

    DIR * dir = opendir(uppmFormulaRepoDIR);

    if (dir == NULL) {
        perror(uppmFormulaRepoDIR);
        return UPPM_ERROR;
    }

    ret = UPPM_OK;

    struct dirent * dir_entry;

    for (;;) {
        errno = 0;

        dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                return UPPM_OK;
            } else {
                perror(uppmFormulaRepoDIR);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        //puts(dir_entry->d_name);

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t formulaRepoPathCapacity = uppmFormulaRepoDIRCapacity + strlen(dir_entry->d_name) + 2U;
        char   formulaRepoPath[formulaRepoPathCapacity];

        ret = snprintf(formulaRepoPath, formulaRepoPathCapacity, "%s/%s", uppmFormulaRepoDIR, dir_entry->d_name);

        if (ret < 0) {
            perror(NULL);
            closedir(dir);
            return UPPM_ERROR;
        }

        size_t formulaRepoConfigFilePathCapacity = formulaRepoPathCapacity + 24U;
        char   formulaRepoConfigFilePath[formulaRepoConfigFilePathCapacity];

        ret = snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathCapacity, "%s/.uppm-formula-repo.yml", formulaRepoPath);

        if (ret < 0) {
            perror(NULL);
            closedir(dir);
            return UPPM_ERROR;
        }

        if (stat(formulaRepoConfigFilePath, &st) != 0) {
            continue;
        }

        UPPMFormulaRepo * formulaRepo = NULL;

        ret = uppm_formula_repo_parse(formulaRepoConfigFilePath, &formulaRepo);

        if (ret != UPPM_OK) {
            uppm_formula_repo_free(formulaRepo);
            closedir(dir);
            return ret;
        }

        formulaRepo->name = strdup(dir_entry->d_name);

        if (formulaRepo->name == NULL) {
            uppm_formula_repo_free(formulaRepo);
            closedir(dir);
            return UPPM_ERROR_MEMORY_ALLOCATE;
        }

        formulaRepo->path = strdup(formulaRepoPath);

        if (formulaRepo->path == NULL) {
            uppm_formula_repo_free(formulaRepo);
            closedir(dir);
            return UPPM_ERROR_MEMORY_ALLOCATE;
        }

        ret = callback(formulaRepo, payload);

        if (ret == 0) {
            continue;
        }

        // callback return -1 means want to stop to scan
        if (ret == -1) {
            uppm_formula_repo_free(formulaRepo);
            closedir(dir);
            return UPPM_OK;
        } else {
            uppm_formula_repo_free(formulaRepo);
            closedir(dir);
            return ret;
        }
    }
}
