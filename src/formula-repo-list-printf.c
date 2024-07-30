#include <stdio.h>

#include <unistd.h>

#include "uppm.h"

static int j = 0;

static int uppm_formula_repo_scan_callback(UPPMFormulaRepo * formulaRepo, const void * payload __attribute__((unused))) {
    if (j > 0) {
        if (isatty(STDOUT_FILENO)) {
            printf("\n");
        } else {
            printf("---\n");
        }
    }

    j++;

    int ret = uppm_formula_repo_info(formulaRepo);

    if (ret == UPPM_OK) {
        uppm_formula_repo_free(formulaRepo);
    }

    return ret;
}

int uppm_formula_repo_list_printf() {
    return uppm_formula_repo_scan(uppm_formula_repo_scan_callback, NULL);
}
