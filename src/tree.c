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

    char   uppmHomeDir[256];
    size_t uppmHomeDirLength;

    ret = uppm_home_dir(uppmHomeDir, 256, &uppmHomeDirLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    size_t   packageInstalledDirLength = uppmHomeDirLength + strlen(packageName) + 12U;
    char     packageInstalledDir[packageInstalledDirLength];
    snprintf(packageInstalledDir, packageInstalledDirLength, "%s/installed/%s", uppmHomeDir, packageName);

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

    size_t   treeCommandPathLength = uppmHomeDirLength + 25U;
    char     treeCommandPath[treeCommandPathLength];
    snprintf(treeCommandPath, treeCommandPathLength, "%s/installed/tree/bin/tree", uppmHomeDir);

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
