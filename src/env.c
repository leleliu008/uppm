#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/utsname.h>

#include "core/fs.h"
#include "uppm.h"

static int uppm_list_dirs(const char * installedDir, size_t installedDirLength, const char * sub) {
    DIR *dir;
    struct dirent *dir_entry;

    dir = opendir(installedDir);

    if (dir == NULL) {
        perror(installedDir);
        return UPPM_ERROR;
    } else {
        while ((dir_entry = readdir(dir))) {
            //puts(dir_entry->d_name);

            if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
                continue;
            }

            size_t  packageInstalledDirLength = installedDirLength + strlen(dir_entry->d_name) + 2;
            char    packageInstalledDir[packageInstalledDirLength];
            memset (packageInstalledDir, 0, packageInstalledDirLength);
            sprintf(packageInstalledDir, "%s/%s", installedDir, dir_entry->d_name);

            size_t  installedMetadataFilePathLength = packageInstalledDirLength + 25;
            char    installedMetadataFilePath[installedMetadataFilePathLength];
            memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
            sprintf(installedMetadataFilePath, "%s/installed-metadata-uppm", packageInstalledDir);

            if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
                if ((sub == NULL) || (strcmp(sub, "") == 0)) {
                    puts(packageInstalledDir);
                } else {
                    size_t  subDirLength = packageInstalledDirLength + strlen(sub) + 2;
                    char    subDir[subDirLength];
                    memset (subDir, 0, subDirLength);
                    sprintf(subDir, "%s/%s", packageInstalledDir, sub);

                    if (exists_and_is_a_directory(subDir)) {
                        puts(subDir);
                    }
                }
            }
        }
        closedir(dir);
    }

    return UPPM_OK;

}

int uppm_env() {
    struct utsname uts;

    if (uname(&uts) < 0) {
        perror("uname() error");
        return UPPM_ERROR;
    } 

    printf("NATIVE_OS_KIND = %s\n", uts.sysname);
    printf("NATIVE_OS_NAME = %s\n", uts.nodename);
    printf("NATIVE_OS_ARCH = %s\n", uts.machine);
    printf("\n");
    char * userHomeDir = getenv("HOME");

    if ((userHomeDir == NULL) || (strcmp(userHomeDir, "") == 0)) {
        fprintf(stderr, "HOME environment variable is not set.\n");
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  uppmHomeDirLength = strlen(userHomeDir) + 7;
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

    printf("UPPM_HOME    = %s\n", uppmHomeDir);
    printf("UPPM_VERSION = %s\n", UPPM_VERSION);

    size_t  installedDirLength = uppmHomeDirLength + 11;
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    sprintf(installedDir, "%s/installed", uppmHomeDir);

    if (!exists_and_is_a_directory(installedDir)) {
        return UPPM_OK;
    }

    printf("\nbinDirs:\n");
    uppm_list_dirs(installedDir, installedDirLength, "bin");

    printf("\nlibDirs:\n");
    uppm_list_dirs(installedDir, installedDirLength, "lib");

    printf("\naclocalDirs:\n");
    uppm_list_dirs(installedDir, installedDirLength, "share/aclocal");
}
