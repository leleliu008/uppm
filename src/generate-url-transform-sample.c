#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_generate_url_transform_sample() {
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

    size_t uppmHomeDirLength = userHomeDirLength + 7;
    char   uppmHomeDir[uppmHomeDirLength];
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    if (stat(uppmHomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmHomeDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t urlTransformSampleFilePathLength = uppmHomeDirLength + 22;
    char   urlTransformSampleFilePath[urlTransformSampleFilePathLength];
    snprintf(urlTransformSampleFilePath, urlTransformSampleFilePathLength, "%s/url-transform.sample", uppmHomeDir);

    FILE * file = fopen(urlTransformSampleFilePath, "w");

    if (file == NULL) {
        perror(urlTransformSampleFilePath);
        return UPPM_ERROR;
    }

    const char * p = ""
        "#!/bin/sh\n"
        "case $1 in\n"
        "    *githubusercontent.com/*)\n"
        "        printf 'https://ghproxy.com/%s\\n' \"$1\"\n"
        "        ;;\n"
        "    https://github.com/*)\n"
        "        printf 'https://ghproxy.com/%s\\n' \"$1\"\n"
        "        ;;\n"
        "    '') printf '%s\\n' \"$0 <URL>, <URL> is unspecified.\" >&2 ;;\n"
        "    *)  printf '%s\\n' \"$1\"\n"
        "esac";

    size_t pSize = strlen(p);

    if (fwrite(p, 1, pSize, file) != pSize || ferror(file)) {
        fclose(file);
        return UPPM_ERROR;
    }

    fclose(file);

    if (chmod(urlTransformSampleFilePath, S_IRWXU) == 0) {
        return UPPM_OK;
    } else {
        perror(urlTransformSampleFilePath);
        return UPPM_ERROR;
    }
}
