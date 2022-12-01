#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "uppm.h"

int uppm_formula_repo_add(const char * formulaRepoUrl, const char * branchName) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  formulaRepoDBPathLength = userHomeDirLength + 16;
    char    formulaRepoDBPath[formulaRepoDBPathLength];
    memset (formulaRepoDBPath, 0, formulaRepoDBPathLength);
    sprintf(formulaRepoDBPath, "%s/.uppm/repos.db", userHomeDir);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sqlite3 * db = NULL;

    int resultCode = sqlite3_open(formulaRepoDBPath, &db);

    if (resultCode != SQLITE_OK) {
        fprintf(stderr, "%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return resultCode;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const char * createTableSql = "CREATE TABLE IF NOT EXISTS formulaRepo (id TEXT PRIMARY KEY, url TEXT NOT NULL, branchName TEXT NOT NULL);";
    char * errorMsg = NULL;

    resultCode = sqlite3_exec(db, createTableSql, NULL, NULL, &errorMsg);

    if (resultCode != SQLITE_OK) {
        fprintf(stderr, "%s\n", errorMsg);
        sqlite3_close(db);
        return resultCode;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char * formulaRepoId = uppm_formula_repo_id(formulaRepoUrl, branchName);

    size_t  updateSqlLength = 80 + strlen(formulaRepoId) + strlen(formulaRepoUrl) + strlen(branchName);
    char    updateSql[updateSqlLength];
    memset( updateSql, 0, updateSqlLength);
    sprintf(updateSql, "INSERT OR REPLACE INTO formulaRepo (id,url,branchName) VALUES ('%s','%s','%s');", formulaRepoId, formulaRepoUrl, branchName);

    errorMsg = NULL;

    resultCode = sqlite3_exec(db, updateSql, NULL, NULL, &errorMsg);

    if (resultCode != SQLITE_OK) {
        fprintf(stderr, "%s\n", errorMsg);
    }

    free(formulaRepoId);
    sqlite3_close(db);
    return resultCode;
}
