#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "core/log.h"
#include "uppm.h"

int uppm_formula_repo_info_(const char * formulaRepoName) {
    UPPMFormulaRepo * formulaRepo = NULL;

    int ret = uppm_formula_repo_lookup(formulaRepoName, &formulaRepo);

    if (ret != UPPM_OK) {
        return ret;
    }

    uppm_formula_repo_info(formulaRepo);
    uppm_formula_repo_free(formulaRepo);

    return UPPM_OK;
}

int uppm_formula_repo_info(UPPMFormulaRepo * formulaRepo) {
    if (formulaRepo == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (isatty(STDOUT_FILENO)) {
        printf("name: %s%s%s\n", COLOR_GREEN, formulaRepo->name, COLOR_OFF);
    } else {
        printf("name: %s\n", formulaRepo->name);
    }

    printf("path: %s\n", formulaRepo->path);
    printf("url:  %s\n", formulaRepo->url);
    printf("branch: %s\n", formulaRepo->branch);
    printf("pinned: %s\n", formulaRepo->pinned ? "yes" : "no");
    printf("enabled: %s\n", formulaRepo->enabled ? "yes" : "no");

    ////////////////////////////////////////////////////////////

    time_t tt = (time_t)atol(formulaRepo->timestamp_created);
    struct tm *tms = localtime(&tt);

    char buff[26] = {0};
    strftime(buff, 26, "%Y-%m-%d %H:%M:%S%z", tms);

    buff[24] = buff[23];
    buff[23] = buff[22];
    buff[22] = ':';

    printf("created: %s\n", buff);

    ////////////////////////////////////////////////////////////

    if (formulaRepo->timestamp_updated != NULL) {
        time_t tt = (time_t)atol(formulaRepo->timestamp_updated);
        struct tm *tms = localtime(&tt);

        char buff[26] = {0};
        strftime(buff, 26, "%Y-%m-%d %H:%M:%S%z", tms);

        buff[24] = buff[23];
        buff[23] = buff[22];
        buff[22] = ':';

        printf("updated: %s\n", buff);
    }

    return UPPM_OK;
}
