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

    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t packageInstalledDirLength = userHomeDirLength + strlen(packageName) + 20U;
    char   packageInstalledDir[packageInstalledDirLength];
    snprintf(packageInstalledDir, packageInstalledDirLength, "%s/.uppm/installed/%s", userHomeDir, packageName);

    struct stat st;

    if (stat(packageInstalledDir, &st) != 0) {
        return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
    }

    size_t receiptFilePathLength = packageInstalledDirLength + 20U;
    char   receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDir);

    if (stat(receiptFilePath, &st) != 0 || (!S_ISREG(st.st_mode))) {
        return UPPM_ERROR_PACKAGE_IS_BROKEN;
    }

    ret = uppm_check_if_the_given_package_is_installed("tree");

    if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
        ret = uppm_install("tree", false);

        if (ret != UPPM_OK) {
            return ret;
        }
    } else if (ret != UPPM_OK) {
        return ret;
    }

    size_t treeCommandPathLength = userHomeDirLength + 31U;
    char   treeCommandPath[treeCommandPathLength];
    snprintf(treeCommandPath, treeCommandPathLength, "%s/.uppm/installed/tree/bin/tree", userHomeDir);

    size_t n = argc + 5U;
    char*  p[n];

    p[0] = treeCommandPath;
    p[1] = (char*)"--dirsfirst";
    p[2] = (char*)"-a";

    for (size_t i = 0; i < argc; i++) {
        p[3+i] = argv[i];
    }

    p[n-2] = packageInstalledDir;
    p[n-1]   = NULL;

    execv(treeCommandPath, p);

    perror(treeCommandPath);

    return UPPM_ERROR;
}
