#include <stdio.h>
#include <string.h>

#include "core/fs.h"
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

    size_t  installedDirLength = userHomeDirLength + strlen(packageName) + 20;
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    snprintf(installedDir, installedDirLength, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t  receiptFilePathLength = installedDirLength + 20;
    char    receiptFilePath[receiptFilePathLength];
    memset (receiptFilePath, 0, receiptFilePathLength);
    snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", installedDir);

    if (exists_and_is_a_regular_file(receiptFilePath)) {
        if (rm_r(installedDir, verbose) == 0) {
            return UPPM_OK;
        } else {
            return UPPM_ERROR;
        }
    }

    return UPPM_PACKAGE_IS_NOT_INSTALLED;
}
