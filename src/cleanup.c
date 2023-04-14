#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "uppm.h"

int uppm_cleanup(bool verbose) {
    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmTmpDirLength = userHomeDirLength + 11U;
    char     uppmTmpDir[uppmTmpDirLength];
    snprintf(uppmTmpDir, uppmTmpDirLength, "%s/.uppm/tmp", userHomeDir);

    if (stat(uppmTmpDir, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (rm_r(uppmTmpDir, verbose) == 0) {
                return UPPM_OK;
            } else {
                perror(uppmTmpDir);
                return UPPM_ERROR;
            }
        } else {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmTmpDir);
            return UPPM_ERROR;
        }
    } else {
        return UPPM_OK;
    }
}
