#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>

#include "core/log.h"

#include "uppm.h"

int uppm_list_the_installed_packages() {
    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    struct stat st;

    size_t   uppmInstalledDIRLength = uppmHomeDIRLength + 11U; 
    char     uppmInstalledDIR[uppmInstalledDIRLength];
    snprintf(uppmInstalledDIR, uppmInstalledDIRLength, "%s/installed", uppmHomeDIR);

    if (stat(uppmInstalledDIR, &st) != 0 || (!S_ISDIR(st.st_mode))) {
        return UPPM_OK;
    }

    DIR * dir = opendir(uppmInstalledDIR);

    if (dir == NULL) {
        perror(uppmInstalledDIR);
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
                perror(uppmInstalledDIR);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t   packageInstalledDIRLength = uppmInstalledDIRLength + strlen(dir_entry->d_name) + 2U;
        char     packageInstalledDIR[packageInstalledDIRLength];
        snprintf(packageInstalledDIR, packageInstalledDIRLength, "%s/%s", uppmInstalledDIR, dir_entry->d_name);

        if (lstat(packageInstalledDIR, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                continue;
            }
        } else {
            continue;
        }

        size_t   receiptFilePathLength = packageInstalledDIRLength + 20U;
        char     receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDIR);

        if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            printf("%s\n", dir_entry->d_name);
        }
    }

    return UPPM_OK;
}
