#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

#include "core/log.h"
#include "uppm.h"

int uppm_list_the_installed_packages() {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    struct stat st;

    size_t  uppmInstalledDirLength = userHomeDirLength + 17; 
    char    uppmInstalledDir[uppmInstalledDirLength];
    memset (uppmInstalledDir, 0, uppmInstalledDirLength);
    snprintf(uppmInstalledDir, uppmInstalledDirLength, "%s/.uppm/installed", userHomeDir);

    if (stat(uppmInstalledDir, &st) != 0 || (!S_ISDIR(st.st_mode))) {
        return UPPM_OK;
    }

    DIR           * dir;
    struct dirent * dir_entry;

    dir = opendir(uppmInstalledDir);

    if (dir == NULL) {
        perror(uppmInstalledDir);
        return UPPM_ERROR;
    }

    while ((dir_entry = readdir(dir))) {
        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t  receiptFilePathLength = uppmInstalledDirLength + strlen(dir_entry->d_name) + 20;
        char    receiptFilePath[receiptFilePathLength];
        memset (receiptFilePath, 0, receiptFilePathLength);
        snprintf(receiptFilePath, receiptFilePathLength, "%s/%s/.uppm/receipt.yml", uppmInstalledDir, dir_entry->d_name);

        if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            printf("%s\n", dir_entry->d_name);
        }
    }

    closedir(dir);

    return UPPM_OK;
}
