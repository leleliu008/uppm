#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/log.h"

#include "uppm.h"

int uppm_list_the_installed_packages(const bool verbose) {
    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    struct stat st;

    size_t uppmInstalledDIRLength = uppmHomeDIRLength + 11U; 
    char   uppmInstalledDIR[uppmInstalledDIRLength];

    ret = snprintf(uppmInstalledDIR, uppmInstalledDIRLength, "%s/installed", uppmHomeDIR);

    if (ret < 0) {
        perror(NULL);
        return ret;
    }

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

        size_t packageInstalledDIRLength = uppmInstalledDIRLength + strlen(dir_entry->d_name) + 2U;
        char   packageInstalledDIR[packageInstalledDIRLength];

        ret = snprintf(packageInstalledDIR, packageInstalledDIRLength, "%s/%s", uppmInstalledDIR, dir_entry->d_name);

        if (ret < 0) {
            perror(NULL);
            closedir(dir);
            return UPPM_ERROR;
        }

        if (lstat(packageInstalledDIR, &st) == 0) {
            if (!S_ISLNK(st.st_mode)) {
                continue;
            }
        } else {
            continue;
        }

        size_t receiptFilePathLength = packageInstalledDIRLength + 20U;
        char   receiptFilePath[receiptFilePathLength];

        ret = snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDIR);

        if (ret < 0) {
            perror(NULL);
            closedir(dir);
            return UPPM_ERROR;
        }

        if (lstat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            if (verbose) {
                puts("===");

                ret = uppm_installed_info(dir_entry->d_name, NULL);

                if (ret != UPPM_OK) {
                    closedir(dir);
                    return ret;
                }
            } else {
                printf("%s\n", dir_entry->d_name);
            }
        }
    }

    return UPPM_OK;
}
