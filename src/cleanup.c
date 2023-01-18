#include <string.h>
#include <sys/stat.h>

#include "uppm.h"
#include "core/log.h"
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

    size_t  uppmTmpDirLength = userHomeDirLength + 11;
    char    uppmTmpDir[uppmTmpDirLength];
    memset (uppmTmpDir, 0, uppmTmpDirLength);
    snprintf(uppmTmpDir, uppmTmpDirLength, "%s/.uppm/tmp", userHomeDir);

    if (stat(uppmTmpDir, &st) != 0) {
        return UPPM_OK;
    }

    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "not a directory: %s\n", uppmTmpDir);
        return UPPM_ERROR;
    }

    return rm_r(uppmTmpDir, verbose);
}
