#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/log.h"

#include "uppm.h"

int uppm_list_the_installed_packages() {
    char   uppmHomeDir[256];
    size_t uppmHomeDirLength;

    int ret = uppm_home_dir(uppmHomeDir, 256, &uppmHomeDirLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    struct stat st;

    size_t   uppmInstalledDirLength = uppmHomeDirLength + 11U; 
    char     uppmInstalledDir[uppmInstalledDirLength];
    snprintf(uppmInstalledDir, uppmInstalledDirLength, "%s/installed", uppmHomeDir);

    if (stat(uppmInstalledDir, &st) != 0 || (!S_ISDIR(st.st_mode))) {
        return UPPM_OK;
    }

    DIR * dir = opendir(uppmInstalledDir);

    if (dir == NULL) {
        perror(uppmInstalledDir);
        return UPPM_ERROR;
    }

    for (;;) {
        errno = 0;

        struct dirent * dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                break;
            } else {
                perror(uppmInstalledDir);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t   receiptFilePathLength = uppmInstalledDirLength + strlen(dir_entry->d_name) + 20U;
        char     receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/%s/.uppm/receipt.yml", uppmInstalledDir, dir_entry->d_name);

        if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            printf("%s\n", dir_entry->d_name);
        }
    }

    return UPPM_OK;
}
