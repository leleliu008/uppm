#include <stdio.h>
#include <string.h>

#include "core/sysinfo.h"

#include "uppm.h"

static int uppm_formula_repo_url_of_official_core(char buf[], const size_t bufSize) {
    char osType[31] = {0};

    if (sysinfo_type(osType, 30) != 0) {
        return UPPM_ERROR;
    }

    char osArch[31] = {0};

    if (sysinfo_arch(osArch, 30) != 0) {
        return UPPM_ERROR;
    }

    char osVers[31] = {0};

    if (sysinfo_vers(osVers, 30) != 0) {
        return UPPM_ERROR;
    }

    int ret;

#if defined (__APPLE__)
    int osVersMajor = 0;

    for (int i = 0; i < 31; i++) {
        if (osVers[i] == '\0') {
            break;
        }

        if (osVers[i] == '.') {
            osVers[i] = '\0';
            osVersMajor = atoi(osVers);
            break;
        }
    }

    if (osVersMajor < 10) {
        fprintf(stderr, "MacOSX %d.x is not supported.\n", osVersMajor);
        return UPPM_ERROR;
    }

    if (osVersMajor > 13) {
        osVersMajor = 13;
    }

    ret = snprintf(buf, bufSize, "https://github.com/leleliu008/uppm-package-repository-%s-%d.0-%s", osType, osVersMajor, osArch);
#elif defined (__DragonFly__)
    ret = snprintf(buf, bufSize, "https://github.com/leleliu008/uppm-package-repository-%s-%s-%s", osType, osVers, osArch);
#elif defined (__FreeBSD__)
    ret = snprintf(buf, bufSize, "https://github.com/leleliu008/uppm-package-repository-%s-%s-%s", osType, osVers, osArch);
#elif defined (__OpenBSD__)
    ret = snprintf(buf, bufSize, "https://github.com/leleliu008/uppm-package-repository-%s-%s-%s", osType, osVers, osArch);
#elif defined (__NetBSD__)
    ret = snprintf(buf, bufSize, "https://github.com/leleliu008/uppm-package-repository-%s-%s-%s", osType, osVers, osArch);
#elif defined (__ANDROID__)
    ret = snprintf(buf, bufSize, "https://github.com/leleliu008/uppm-package-repository-%s-%s-%s", osType, osVers, osArch);
#else
    ret = snprintf(buf, bufSize, "https://github.com/leleliu008/uppm-formula-repository-%s-%s", osType, osArch);
#endif

    if (ret < 0) {
        perror(NULL);
        return UPPM_ERROR;
    } else {
        return UPPM_OK;
    }
}

int uppm_formula_repo_list_update() {
    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret != UPPM_OK) {
        return ret;
    }

    bool officialCoreIsThere = false;

    for (size_t i = 0U; i < formulaRepoList->size; i++) {
        UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];

        if (strcmp(formulaRepo->name, "official-core") == 0) {
            officialCoreIsThere = true;
        }

        ret = uppm_formula_repo_sync(formulaRepo);

        if (ret != UPPM_OK) {
            break;
        }
    }

    uppm_formula_repo_list_free(formulaRepoList);

    if (officialCoreIsThere) {
        return UPPM_OK;
    }

    char formulaRepoUrl[120];

    ret = uppm_formula_repo_url_of_official_core(formulaRepoUrl, 120);

    if (ret != UPPM_OK) {
        return ret;
    }

    return uppm_formula_repo_add("official-core", formulaRepoUrl, "master", false, true);
}
