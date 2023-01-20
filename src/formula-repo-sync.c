#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "core/log.h"
#include "uppm.h"

int uppm_formula_repo_sync(UPPMFormulaRepo * formulaRepo) {
    if (formulaRepo == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (formulaRepo->pinned) {
        fprintf(stderr, "'%s' formula repo was pinned, skipped.\n", formulaRepo->name);
        return UPPM_OK;
    }

    if (isatty(STDOUT_FILENO)) {
        printf("%s%s%s\n", COLOR_PURPLE, "==> Updating formula repo", COLOR_OFF);
    } else {
        printf("=== Updating formula repo\n");
    }

    uppm_formula_repo_dump(formulaRepo);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t refspecLength = (strlen(formulaRepo->branch) << 1) + 33;
    char   refspec[refspecLength];
    snprintf(refspec, refspecLength, "refs/heads/%s:refs/remotes/origin/%s", formulaRepo->branch, formulaRepo->branch);

    if (uppm_fetch_via_git(formulaRepo->path, formulaRepo->url, refspec, formulaRepo->branch) != 0) {
        return UPPM_ERROR;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char ts[11];
    snprintf(ts, 11, "%ld", time(NULL));

    size_t strLength = strlen(formulaRepo->url) + strlen(formulaRepo->branch) + 99;
    char   str[strLength];
    snprintf(str, strLength, "url: %s\nbranch: %s\npinned: %1d\nenabled: %1d\ntimestamp-added: %10s\ntimestamp-last-updated: %10s\n", formulaRepo->url, formulaRepo->branch, formulaRepo->pinned, formulaRepo->enabled, formulaRepo->timestamp_added, ts);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t formulaRepoConfigFilePathLength = strlen(formulaRepo->path) + 24;
    char   formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    snprintf(formulaRepoConfigFilePath, formulaRepoConfigFilePathLength, "%s/.uppm-formula-repo.yml", formulaRepo->path);

    FILE * file = fopen(formulaRepoConfigFilePath, "w");

    if (file == NULL) {
        perror(formulaRepoConfigFilePath);
        return UPPM_ERROR;
    }

    if (fwrite(str, 1, strLength, file) != strLength || ferror(file)) {
        perror(formulaRepoConfigFilePath);
        fclose(file);
        return UPPM_ERROR;
    }

    fclose(file);

    return UPPM_OK;
}
