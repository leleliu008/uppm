#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "uppm.h"

int uppm_cleanup(bool verbose) {
    char   uppmHomeDir[256];
    size_t uppmHomeDirLength;

    int ret = uppm_home_dir(uppmHomeDir, 256, &uppmHomeDirLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmTmpDirLength = uppmHomeDirLength + 5U;
    char     uppmTmpDir[uppmTmpDirLength];
    snprintf(uppmTmpDir, uppmTmpDirLength, "%s/tmp", uppmHomeDir);

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
