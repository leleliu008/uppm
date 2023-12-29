#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/self.h"

#include "uppm.h"

static int uppm_list_dirs(const char * installedDIR, size_t installedDIRLength, const char * sub) {
    DIR * dir = opendir(installedDIR);

    if (dir == NULL) {
        perror(installedDIR);
        return UPPM_ERROR;
    }

    struct stat st;

    for (;;) {
        errno = 0;

        struct dirent * dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                break;
            } else {
                perror(installedDIR);
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        //puts(dir_entry->d_name);

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t packageInstalledDIRLength = installedDIRLength + strlen(dir_entry->d_name) + 2U;
        char   packageInstalledDIR[packageInstalledDIRLength];
        snprintf(packageInstalledDIR, packageInstalledDIRLength, "%s/%s", installedDIR, dir_entry->d_name);

        size_t receiptFilePathLength = packageInstalledDIRLength + 20U;
        char   receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/.uppm/receipt.yml", packageInstalledDIR);

        if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            if ((sub == NULL) || (sub[0] == '\0')) {
                printf("%s\n", packageInstalledDIR);
            } else {
                size_t subDIRLength = packageInstalledDIRLength + strlen(sub) + 2U;
                char   subDIR[subDIRLength];
                snprintf(subDIR, subDIRLength, "%s/%s", packageInstalledDIR, sub);

                if (stat(subDIR, &st) == 0 && S_ISDIR(st.st_mode)) {
                    printf("%s\n", subDIR);
                }
            }
        }
    }

    return UPPM_OK;
}

int uppm_env(const bool verbose) {
    char   uppmHomeDIR[PATH_MAX];
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, PATH_MAX, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    printf("uppm.version : %s\n", UPPM_VERSION);
    printf("uppm.homedir : %s\n", uppmHomeDIR);

    char * selfRealPath = self_realpath();

    if (selfRealPath == NULL) {
        perror(NULL);
        return UPPM_ERROR;
    }

    printf("uppm.exepath : %s\n", selfRealPath);

    free(selfRealPath);

    printf("uppm.website : %s\n", "https://github.com/leleliu008/uppm");

    if (!verbose) {
        return UPPM_OK;
    }

    struct stat st;

    size_t   installedDIRLength = uppmHomeDIRLength + 11U;
    char     installedDIR[installedDIRLength];
    snprintf(installedDIR, installedDIRLength, "%s/installed", uppmHomeDIR);

    if (stat(installedDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "%s was expected to be a directory, but it was not.\n", installedDIR);
            return UPPM_ERROR;
        }
    } else {
        return UPPM_OK;
    }

    printf("\nbinDIRs:\n");
    uppm_list_dirs(installedDIR, installedDIRLength, "bin");

    printf("\nlibDIRs:\n");
    uppm_list_dirs(installedDIR, installedDIRLength, "lib");

    printf("\naclocalDIRs:\n");
    uppm_list_dirs(installedDIR, installedDIRLength, "share/aclocal");
    
    return UPPM_OK;
}
