#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "uppm.h"
#include "core/rm-r.h"

int uppm_cleanup(bool verbose) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t uppmTmpDirLength = userHomeDirLength + 11;
    char   uppmTmpDir[uppmTmpDirLength];
    snprintf(uppmTmpDir, uppmTmpDirLength, "%s/.uppm/tmp", userHomeDir);

    if (stat(uppmTmpDir, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return rm_r(uppmTmpDir, verbose);
        } else {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmTmpDir);
            return UPPM_ERROR;
        }
    } else {
        return UPPM_OK;
    }
}
