#include <stdio.h>

#include <limits.h>

#include "core/self.h"

#include "uppm.h"

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
   
    return UPPM_OK;
}
