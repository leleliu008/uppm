#include <stdio.h>
#include <string.h>

#include "core/fs.h"
#include "core/rm-r.h"
#include "uppm.h"

int uppm_uninstall(const char * packageName) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  installedDirLength = userHomeDirLength + strlen(packageName) + 20;
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    sprintf(installedDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t  installedMetadataFilePathLength = installedDirLength + 26;
    char    installedMetadataFilePath[installedMetadataFilePathLength];
    memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
    sprintf(installedMetadataFilePath, "%s/installed-metadata-uppm", installedDir);

    if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
        if (rm_r(installedDir) == 0) {
            return UPPM_OK;
        } else {
            return UPPM_ERROR;
        }
    }

    return UPPM_PACKAGE_IS_NOT_INSTALLED;
}
