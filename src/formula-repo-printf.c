#include "uppm.h"

int uppm_formula_repo_printf(const char * formulaRepoName) {
    UPPMFormulaRepo * formulaRepo = NULL;

    int ret = uppm_formula_repo_lookup(formulaRepoName, &formulaRepo);

    if (ret == UPPM_OK) {
        uppm_formula_repo_dump(formulaRepo);
    }

    return ret;
}
