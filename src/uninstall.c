#include <stdio.h>
#include <string.h>

#include "fs.h"
#include "rm-r.h"
#include "uppm.h"

int uppm_uninstall(const char * pkgName) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  installDirLength = userHomeDirLength + strlen(pkgName) + 20;
    char    installDir[installDirLength];
    memset (installDir, 0, installDirLength);
    sprintf(installDir, "%s/.uppm/installed/%s", userHomeDir, pkgName);

    size_t  installedMetadataFilePathLength = installDirLength + 26;
    char    installedMetadataFilePath[installedMetadataFilePathLength];
    memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
    sprintf(installedMetadataFilePath, "%s/uppm-installed-metadata", installDir);

    if (exists_and_is_a_directory(installDir)) {
        if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
            if (rm_r(installDir) == 0) {
                return UPPM_OK;
            } else {
                return UPPM_ERROR;
            }
        }
    }

    fprintf(stderr, "package [%s] is not installed.\n", pkgName);
    return UPPM_PACKAGE_IS_NOT_INSTALLED;
}
