#include <stdio.h>
#include <string.h>

#include "core/sysinfo.h"
#include "uppm.h"

int uppm_formula_repo_list_update() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int resultCode = uppm_formula_repo_list(&formulaRepoList);

    if (resultCode == UPPM_OK) {
        bool officalCoreIsThere = false;

        for (size_t i = 0; i < formulaRepoList->size; i++) {
            UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];

            if (strcmp(formulaRepo->name, "offical-core") == 0) {
                officalCoreIsThere = true;
            }

            if (formulaRepo->pinned) {
                fprintf(stderr, "[%s] formula repo was pinned, skipped.\n", formulaRepo->name);
            } else {
                resultCode = uppm_formula_repo_update(formulaRepo);

                if (resultCode != UPPM_OK) {
                    break;
                }
            }
        }

        uppm_formula_repo_list_free(formulaRepoList);

        if (!officalCoreIsThere) {
            char osType[31] = {0};

            resultCode = sysinfo_type(osType, 30);

            if (resultCode != UPPM_OK) {
                return resultCode;
            }

            char osArch[31] = {0};

            resultCode = sysinfo_arch(osArch, 30);

            if (resultCode != UPPM_OK) {
                return resultCode;
            }

            size_t formulaRepoUrlLength = strlen(osType) + strlen(osArch) + 56;
            char   formulaRepoUrl[formulaRepoUrlLength];
            memset(formulaRepoUrl, 0, formulaRepoUrlLength);
            snprintf(formulaRepoUrl, formulaRepoUrlLength, "https://github.com/leleliu008/uppm-formula-repository-%s-%s", osType, osArch);

            resultCode = uppm_formula_repo_add("offical-core", formulaRepoUrl, "master");
        }
    }

    return resultCode;
}
