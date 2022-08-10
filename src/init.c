#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/utsname.h>

#include "core/fs.h"
#include "uppm.h"

int uppm_init() {
    struct utsname uts;

    if (uname(&uts) < 0) {
        perror("uname() error");
        return UPPM_ERROR;
    } 

    setenv("NATIVE_OS_KIND", uts.sysname,  1);
    setenv("NATIVE_OS_NAME", uts.nodename, 1);
    setenv("NATIVE_OS_ARCH", uts.machine,  1);

    char * userHomeDir = getenv("HOME");

    if ((userHomeDir == NULL) || (strcmp(userHomeDir, "") == 0)) {
        fprintf(stderr, "HOME environment variable is not set.\n");
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  uppmHomeDirLength = strlen(userHomeDir) + 7;
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

    setenv("UPPM_HOME", uppmHomeDir, 1);
    setenv("UPPM_VERSION", UPPM_VERSION, 1);


    size_t  installedDirLength = uppmHomeDirLength + 11;
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    sprintf(installedDir, "%s/installed", uppmHomeDir);

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
            size_t  packageInstalledDirLength = installedDirLength + strlen(dir_entry->d_name) + 2;
            char    packageInstalledDir[packageInstalledDirLength];
            memset (packageInstalledDir, 0, packageInstalledDirLength);
            sprintf(packageInstalledDir, "%s/%s", installedDir, dir_entry->d_name);

            size_t  installedMetadataFilePathLength = packageInstalledDirLength + 25;
            char    installedMetadataFilePath[installedMetadataFilePathLength];
            memset (installedMetadataFilePath, 0, installedMetadataFilePathLength);
            sprintf(installedMetadataFilePath, "%s/uppm-installed-metadata", packageInstalledDir);

            if (exists_and_is_a_regular_file(installedMetadataFilePath)) {
                const char * PATH = getenv("PATH");

                size_t  binDirLength = packageInstalledDirLength + 5;
                char    binDir[binDirLength];
                memset (binDir, 0, binDirLength);
                sprintf(binDir, "%s/bin", packageInstalledDir);

                if (exists_and_is_a_directory(binDir)) {
                    size_t  newPATHLength = packageInstalledDirLength + binDirLength + strlen(PATH) + 3;
                    char    newPATH[newPATHLength];
                    memset (newPATH, 0, newPATHLength);
                    sprintf(newPATH, "%s:%s:%s", packageInstalledDir, binDir, PATH);

                    setenv("PATH", newPATH, 1);
                } else {
                    size_t  newPATHLength = packageInstalledDirLength + strlen(PATH) + 2;
                    char    newPATH[newPATHLength];
                    memset (newPATH, 0, newPATHLength);
                    sprintf(newPATH, "%s:%s", packageInstalledDir, PATH);

                    setenv("PATH", newPATH, 1);
                }

                size_t  aclocalDirLength = packageInstalledDirLength + 15;
                char    aclocalDir[aclocalDirLength];
                memset (aclocalDir, 0, aclocalDirLength);
                sprintf(aclocalDir, "%s/share/aclocal", packageInstalledDir);

                if (exists_and_is_a_directory(aclocalDir)) {
                    const char * ACLOCAL_PATH = getenv("ACLOCAL_PATH");

                    if ((ACLOCAL_PATH == NULL) || (strcmp(ACLOCAL_PATH, "") == 0)) {
                        setenv("ACLOCAL_PATH", aclocalDir, 1);
                    } else {
                        size_t  newACLOCAL_PATHLength = aclocalDirLength + strlen(ACLOCAL_PATH);
                        char    newACLOCAL_PATH[newACLOCAL_PATHLength];
                        memset (newACLOCAL_PATH, 0, newACLOCAL_PATHLength);
                        sprintf(newACLOCAL_PATH, "%s:%s", installedDir, PATH);

                        setenv("ACLOCAL_PATH", newACLOCAL_PATH, 1);
                    }
                }
            }
        }
        closedir(dir);
    }

    return UPPM_OK;
}
