#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/rm-r.h"
#include "uppm.h"

int uppm_uninstall(const char * packageName, bool verbose) {
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
    snprintf(packageInstalledDir, packageInstalledDirLength, "%s/.uppm/installed/%s", userHomeDir, packageName);

    struct stat st;

    if (stat(packageInstalledDir, &st) != 0) {
        return UPPM_PACKAGE_IS_NOT_INSTALLED;
    }

    size_t  receiptFilePathLength = packageInstalledDirLength + 20;
    char    receiptFilePath[receiptFilePathLength];
    memset (receiptFilePath, 0, receiptFilePathLength);
    snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDir);

    if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        if (rm_r(packageInstalledDir, verbose) == 0) {
            return UPPM_OK;
        } else {
            return UPPM_ERROR;
        }
    } else {
        // package is broken. is not installed completely?
        return UPPM_PACKAGE_IS_NOT_INSTALLED;
    }
}
