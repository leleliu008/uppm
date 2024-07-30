#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

#include "uppm.h"

typedef struct {
    const char * packageName;
    size_t       packageNameLength;

    char   * buf;
    size_t * len;
} UPPMArgs;

static int uppm_formula_repo_scan_callback(UPPMFormulaRepo * formulaRepo, const void * payload) {
    UPPMArgs * args = (UPPMArgs*) payload;

    char * formulaRepoPath = formulaRepo->path;

    size_t formulaFilePathLength = strlen(formulaRepoPath) + args->packageNameLength + 15U;
    char   formulaFilePath[formulaFilePathLength];

    int ret = snprintf(formulaFilePath, formulaFilePathLength, "%s/formula/%s.yml", formulaRepoPath, args->packageName);

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    }

    struct stat st;

    if (stat(formulaFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        strncpy(args->buf, formulaFilePath, ret);

        args->buf[ret] = '\0';

        if (args->len != NULL) {
            (*(args->len)) = ret;
        }

        return UPPM_OK;
    }
}

int uppm_formula_path(const char * packageName, char buf[], size_t * len) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    UPPMArgs args = { .packageName = packageName, .packageNameLength = strlen(packageName), .buf = buf, .len = len };

    return uppm_formula_repo_scan(uppm_formula_repo_scan_callback, &args);
}
