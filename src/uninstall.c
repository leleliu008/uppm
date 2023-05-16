#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "uppm.h"

int uppm_uninstall(const char * packageName, bool verbose) {
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

    size_t   receiptFilePathLength = packageInstalledDirLength + 20U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDir);

    if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        if (rm_r(packageInstalledDir, verbose) == 0) {
            return UPPM_OK;
        } else {
            perror(packageInstalledDir);
            return UPPM_ERROR;
        }
    } else {
        // package is broken. is not installed completely?
        return UPPM_ERROR_PACKAGE_NOT_INSTALLED;
    }
}
