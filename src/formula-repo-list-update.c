#include <stdio.h>
#include <string.h>

#include "core/sysinfo.h"

#include "uppm.h"

int uppm_formula_repo_list_update() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret == UPPM_OK) {
        bool officalCoreIsThere = false;

        for (size_t i = 0U; i < formulaRepoList->size; i++) {
            UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];

            if (strcmp(formulaRepo->name, "offical-core") == 0) {
                officalCoreIsThere = true;
            }

            ret = uppm_formula_repo_sync(formulaRepo);

            if (ret != UPPM_OK) {
                break;
            }
        }

        uppm_formula_repo_list_free(formulaRepoList);

        if (!officalCoreIsThere) {
            char osType[31] = {0};

            if (sysinfo_type(osType, 30) != 0) {
                return UPPM_ERROR;
            }

            char osArch[31] = {0};

            if (sysinfo_arch(osArch, 30) != 0) {
                return UPPM_ERROR;
            }

            char formulaRepoUrl[120];

            #if defined (__APPLE__) || defined (__DragonFly__) || defined (__FreeBSD__) || defined (__OpenBSD__) || defined (__NetBSD__)
                char osVers[31] = {0};

                if (sysinfo_vers(osArch, 30) != 0) {
                    return UPPM_ERROR;
                }

                #if defined (__APPLE__)
                    for (int i = 0; i < 31; i++) {
                        if (osVers[i] == '\0') {
                            break;
                        }

                        if (osVers[i] == '.') {
                            if (osVers[i + 1] != '\0') {
                                osVers[i + 1] = '0';
                                osVers[i + 2] = '\0';
                                break;
                            }
                        }
                    }
                #endif

                ret = snprintf(formulaRepoUrl, 120, "https://github.com/leleliu008/uppm-formula-repository-%s-%s-%s", osType, osVers, osArch);
            #else
                ret = snprintf(formulaRepoUrl, 120, "https://github.com/leleliu008/uppm-formula-repository-%s-%s", osType, osArch);
            #endif

            if (ret < 0) {
                perror(NULL);
                return UPPM_ERROR;
            }

            ret = uppm_formula_repo_add("offical-core", formulaRepoUrl, "master", false, true);
        }
    }

    return ret;
}
