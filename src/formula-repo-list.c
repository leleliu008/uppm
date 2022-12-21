#include <stdio.h>
#include <string.h>

#include "core/fs.h"
#include "core/sysinfo.h"
#include "core/sha256sum.h"
#include "uppm.h"

#include <sqlite3.h>

static bool check_os_kind_arch_is_supported(const char * osKind, const char * osArch) {
    if (strcmp(osArch, "x86_64") == 0) {
        return true;
    } else if (strcmp(osArch, "arm64") == 0) {
        return true;
    } else {
        fprintf(stderr, "%s/%s is not supported yet.", osKind, osArch);
        return false;
    }
}

UPPMFormulaRepo* uppm_formula_repo_default_new(char * userHomeDir, size_t userHomeDirLength) {
    char * osType = NULL;
    char * osArch = NULL;

    sysinfo_type(&osType);
    sysinfo_arch(&osArch);

    char *  formulaRepoUrl = (char*)calloc(strlen(osType) + strlen(osArch) + 56, sizeof(char));
    sprintf(formulaRepoUrl, "https://github.com/leleliu008/uppm-formula-repository-%s-%s", osType, osArch);

    free(osType);
    free(osArch);

    char *  formulaRepoPath = (char*)calloc(userHomeDirLength + 80, sizeof(char));
    sprintf(formulaRepoPath, "%s/.uppm/repos.d/offical-core", userHomeDir);

    UPPMFormulaRepo * formulaRepo = (UPPMFormulaRepo*)calloc(1, sizeof(UPPMFormulaRepo));
    formulaRepo->branch = strdup("master");
    formulaRepo->name   = strdup("offical-core");
    formulaRepo->url    = formulaRepoUrl;
    formulaRepo->path   = formulaRepoPath;

    return formulaRepo;
}

int uppm_formula_repo_list_new(UPPMFormulaRepoList * * out) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  formulaRepoDBPathLength = userHomeDirLength + 16;
    char    formulaRepoDBPath[formulaRepoDBPathLength];
    memset (formulaRepoDBPath, 0, formulaRepoDBPathLength);
    sprintf(formulaRepoDBPath, "%s/.uppm/repos.db", userHomeDir);

    UPPMFormulaRepoList * formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));
    formulaRepoList->repos = (UPPMFormulaRepo**)calloc(1, sizeof(UPPMFormulaRepo*));
    formulaRepoList->repos[0] = uppm_formula_repo_default_new(userHomeDir, userHomeDirLength);
    formulaRepoList->size     = 1;

    if (!exists_and_is_a_regular_file(formulaRepoDBPath)) {
        (*out) = formulaRepoList;
        return UPPM_OK;
    }

    size_t capacity = 5;

    sqlite3      * db        = NULL;
    sqlite3_stmt * statement = NULL;

    int resultCode = sqlite3_open(formulaRepoDBPath, &db);

    if (resultCode != SQLITE_OK) {
        goto clean;
    }

    resultCode = sqlite3_prepare_v2(db, "select * from formulaRepo", -1, &statement, NULL);

    if (resultCode != SQLITE_OK) {
        if (strcmp(sqlite3_errmsg(db), "Error: no such table: formulaRepo") == 0) {
            resultCode = SQLITE_OK;
        }
        goto clean;
    }

    for(;;) {
        resultCode = sqlite3_step(statement);

        if (resultCode == SQLITE_ROW) {
            resultCode =  SQLITE_OK;

            char * formulaRepoName   = (char *)sqlite3_column_text(statement, 0);
            char * formulaRepoUrl    = (char *)sqlite3_column_text(statement, 1);
            char * formulaRepoBranch = (char *)sqlite3_column_text(statement, 2);

            //printf("formulaRepoName=%s\nformulaRepoUrl=%s\nformulaRepoBranch=%s\n", formulaRepoName, formulaRepoUrl, formulaRepoBranch);

            char *  formulaRepoPath = (char*)calloc(userHomeDirLength + 16 + strlen(formulaRepoName), sizeof(char));
            sprintf(formulaRepoPath, "%s/.uppm/repos.d/%s", userHomeDir, formulaRepoName);

            UPPMFormulaRepo * formulaRepo = (UPPMFormulaRepo*)calloc(1, sizeof(UPPMFormulaRepo));
            formulaRepo->name = strdup(formulaRepoName);
            formulaRepo->url  = strdup(formulaRepoUrl);
            formulaRepo->path =        formulaRepoPath;
            formulaRepo->branch  = strdup(formulaRepoBranch);

            if (formulaRepoList->size == capacity) {
                capacity += capacity;
                formulaRepoList->repos = (UPPMFormulaRepo**)realloc(formulaRepoList->repos, capacity * sizeof(UPPMFormulaRepo*));
            }

            formulaRepoList->repos[formulaRepoList->size] = formulaRepo;
            formulaRepoList->size += 1;

            continue;
        }

        if (resultCode == SQLITE_DONE) {
            resultCode =  SQLITE_OK;
        }

        break;
    }

clean:
    if (resultCode == SQLITE_OK) {
        if (formulaRepoList == NULL) {
            formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));
            formulaRepoList->repos = (UPPMFormulaRepo**)calloc(1, sizeof(UPPMFormulaRepo*));
            formulaRepoList->repos[0] = uppm_formula_repo_default_new(userHomeDir, userHomeDirLength);
            formulaRepoList->size     = 1;
        }

        (*out) = formulaRepoList;
    } else {
        fprintf(stderr, "%s\n", sqlite3_errmsg(db));
    }

    if (statement != NULL) {
        sqlite3_finalize(statement);
    }

    if (db != NULL) {
        sqlite3_close(db);
    }

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

        free(formulaRepo->name);
        free(formulaRepo->url);
        free(formulaRepo->path);
        free(formulaRepo->branch);

        formulaRepo->name = NULL;
        formulaRepo->url  = NULL;
        formulaRepo->path = NULL;
        formulaRepo->branch = NULL;

        free(formulaRepoList->repos[i]);
        formulaRepoList->repos[i] = NULL;
    }

    free(formulaRepoList->repos);
    formulaRepoList->repos = NULL;

    free(formulaRepoList);
}
