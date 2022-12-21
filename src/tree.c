#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "core/fs.h"
#include "uppm.h"

int uppm_tree(const char * packageName, size_t argc, char* argv[]) {
    int resultCode = uppm_check_if_the_given_argument_matches_package_name_pattern(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  packageInstalledDirLength = userHomeDirLength + strlen(packageName) + 20;
    char    packageInstalledDir[packageInstalledDirLength];
    memset (packageInstalledDir, 0, packageInstalledDirLength);
    sprintf(packageInstalledDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t  receiptFilePathLength = packageInstalledDirLength + 20;
    char    receiptFilePath[receiptFilePathLength];
    memset (receiptFilePath, 0, receiptFilePathLength);
    sprintf(receiptFilePath, "%s/.uppm/receipt.yml", packageInstalledDir);

    if (!exists_and_is_a_regular_file(receiptFilePath)) {
        return UPPM_PACKAGE_IS_NOT_INSTALLED;
    }

    resultCode = uppm_check_if_the_given_package_is_installed("tree");

    if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
        resultCode = uppm_install("tree", false);

        if (resultCode != UPPM_OK) {
            return resultCode;
        }
    } else if (resultCode != UPPM_OK) {
        return resultCode;
    }

    size_t  treeCommandPathLength = userHomeDirLength + 31;
    char    treeCommandPath[treeCommandPathLength];
    memset (treeCommandPath, 0, treeCommandPathLength);
    sprintf(treeCommandPath, "%s/.uppm/installed/tree/bin/tree", userHomeDir);

    size_t n = argc + 5;
    char*  p[n];

    p[0] = treeCommandPath;
    p[1] = (char*)"--dirsfirst";
    p[2] = (char*)"-a";

    for (size_t i = 0; i < argc; i++) {
        p[3+i] = argv[i];
    }

    p[n-2] = packageInstalledDir;
    p[n-1]   = NULL;

    if (execv(treeCommandPath, p) == -1) {
        perror(treeCommandPath);
        return UPPM_ERROR;
    } else {
        return UPPM_OK;
    }
}
