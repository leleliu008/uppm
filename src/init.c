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

    return UPPM_OK;
}
