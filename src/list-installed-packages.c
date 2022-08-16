#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>

#include "core/log.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_list_the_installed_packages() {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  installedDirLength = userHomeDirLength + 17; 
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    sprintf(installedDir, "%s/.uppm/installed", userHomeDir);

    if (!exists_and_is_a_directory(installedDir)) {
        return UPPM_OK;
    }

    DIR *dir;
    struct dirent *dir_entry;

    dir = opendir(installedDir);

    if (dir == NULL) {
        perror(installedDir);
        return UPPM_ERROR;
    } else {
        while ((dir_entry = readdir(dir))) {
            size_t  installedMetadataFilePathLength = installedDirLength + strlen(dir_entry->d_name) + 26;
            char    installedMetadataFilePath[installedMetadataFilePathLength];
            memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
            sprintf(installedMetadataFilePath, "%s/%s/installed-metadata-uppm", installedDir, dir_entry->d_name);

            if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
                printf("%s\n", dir_entry->d_name);
            }
        }
        closedir(dir);
    }

    return UPPM_OK;
}
