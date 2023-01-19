#include <stdio.h>
#include <string.h>

#include "core/sysinfo.h"
#include "uppm.h"

int uppm_formula_repo_list_update() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret == UPPM_OK) {
        bool officalCoreIsThere = false;

        for (size_t i = 0; i < formulaRepoList->size; i++) {
            UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];

            if (strcmp(formulaRepo->name, "offical-core") == 0) {
                officalCoreIsThere = true;
            }

            if (formulaRepo->pinned) {
                fprintf(stderr, "[%s] formula repo was pinned, skipped.\n", formulaRepo->name);
            } else {
                ret = uppm_formula_repo_sync(formulaRepo);

                if (ret != UPPM_OK) {
                    break;
                }
            }
        }

        uppm_formula_repo_list_free(formulaRepoList);

        if (!officalCoreIsThere) {
            char osType[31] = {0};

            ret = sysinfo_type(osType, 30);

            if (ret != UPPM_OK) {
                return ret;
            }

            char osArch[31] = {0};

            ret = sysinfo_arch(osArch, 30);

            if (ret != UPPM_OK) {
                return ret;
            }

            size_t formulaRepoUrlLength = strlen(osType) + strlen(osArch) + 56;
            char   formulaRepoUrl[formulaRepoUrlLength];
            memset(formulaRepoUrl, 0, formulaRepoUrlLength);
            snprintf(formulaRepoUrl, formulaRepoUrlLength, "https://github.com/leleliu008/uppm-formula-repository-%s-%s", osType, osArch);

            ret = uppm_formula_repo_add("offical-core", formulaRepoUrl, "master");
        }
    }

    return ret;
}
