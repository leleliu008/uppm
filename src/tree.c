#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "core/fs.h"
#include "uppm.h"

int uppm_tree(const char * packageName) {
    int resultCode = uppm_is_package_name(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  installDirLength = userHomeDirLength + strlen(packageName) + 20;
    char    installDir[installDirLength];
    memset (installDir, 0, installDirLength);
    sprintf(installDir, "%s/.uppm/installed/%s", userHomeDir, packageName);

    size_t  installedMetadataFilePathLength = installDirLength + 25;
    char    installedMetadataFilePath[installedMetadataFilePathLength];
    memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
    sprintf(installedMetadataFilePath, "%s/installed-metadata-uppm", installDir);

    if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
        if (execlp("tree", "tree", "--dirsfirst", installDir, NULL) == -1) {
            fprintf(stderr, "command not found: tree\n");
            return UPPM_ERROR;
        } else {
            return UPPM_OK;
        }
    } else {
        return UPPM_PACKAGE_IS_NOT_INSTALLED;
    }
}
