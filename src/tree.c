#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_tree(const char * packageName, size_t argc, char* argv[]) {
    int ret = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t   packageInstalledDIRLength = uppmHomeDIRLength + strlen(packageName) + 12U;
    char     packageInstalledDIR[packageInstalledDIRLength];
    snprintf(packageInstalledDIR, packageInstalledDIRLength, "%s/installed/%s", uppmHomeDIR, packageName);

    struct stat st;

    if (stat(packageInstalledDIR, &st) != 0) {
        return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
    }

    size_t   receiptFilePathLength = packageInstalledDIRLength + 20U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDIR);

    if (stat(receiptFilePath, &st) != 0 || (!S_ISREG(st.st_mode))) {
        return UPPM_ERROR_PACKAGE_IS_BROKEN;
    }

    ret = uppm_check_if_the_given_package_is_installed("tree");

    if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
        ret = uppm_install("tree", false, true);

        if (ret != UPPM_OK) {
            return ret;
        }
    } else if (ret != UPPM_OK) {
        return ret;
    }

    size_t   treeCommandPathLength = uppmHomeDIRLength + 25U;
    char     treeCommandPath[treeCommandPathLength];
    snprintf(treeCommandPath, treeCommandPathLength, "%s/installed/tree/bin/tree", uppmHomeDIR);

    size_t n = argc + 5U;
    char*  p[n];

    p[0] = treeCommandPath;
    p[1] = (char*)"--dirsfirst";
    p[2] = (char*)"-a";

    for (size_t i = 0U; i < argc; i++) {
        p[3U + i] = argv[i];
    }

    p[n - 2U] = packageInstalledDIR;
    p[n - 1U]   = NULL;

    execv(treeCommandPath, p);

    perror(treeCommandPath);

    return UPPM_ERROR;
}
