#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_url_transform(const char * url, char ** out, bool verbose) {
    char * urlTransformCommandPath = getenv("UPPM_URL_TRANSFORM");

    if (urlTransformCommandPath == NULL) {
        return UPPM_ERROR_URL_TRANSFORM_ENV_NOT_SET;
    }

    if (strcmp(urlTransformCommandPath, "") == 0) {
        return UPPM_ERROR_URL_TRANSFORM_ENV_VALUE_IS_EMPTY;
    }

    struct stat st;

    if ((stat(urlTransformCommandPath, &st) == 0) && (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode))) {
        ;
    } else {
        return UPPM_ERROR_URL_TRANSFORM_ENV_POINT_TO_PATH_NOT_EXIST;
    }

    size_t urlLength = strlen(url);

    size_t cmdLength = urlLength + strlen(urlTransformCommandPath) + 2;
    char   cmd[cmdLength];
    snprintf(cmd, cmdLength, "%s %s", urlTransformCommandPath, url);

    FILE * file = popen(cmd, "r");

    if (file == NULL) {
        perror(cmd);
        return UPPM_ERROR;
    }

    size_t capcity = 0;
    size_t size    = 0;
    char * result = NULL;
    char   c;

    for (;;) {
        c = fgetc(file);

        if (c == EOF) {
            break;
        }

        if (c == '\n') {
            break;
        }

        if (capcity == size) {
            capcity += 256;

            char * ptr = (char*)realloc(result, capcity * sizeof(char));

            if (ptr == NULL) {
                free(result);
                pclose(file);
                return UPPM_ERROR_MEMORY_ALLOCATE;
            }

            memset(&ptr[size], 0, 256);

            result = ptr;
        }

        result[size] = c;
        size++;
    }

    pclose(file);

    if (result == NULL) {
        return UPPM_ERROR_URL_TRANSFORM_RUN_NO_RESULT;
    } else {
        if (verbose) {
            fprintf(stderr, "\nyou have set UPPM_URL_TRANSFORM=%s\n", urlTransformCommandPath);
            fprintf(stderr, "transform from: %s\n", url);
            fprintf(stderr, "transform to:   %s\n\n", result);
        }

        (*out) = result;
        return UPPM_OK;
    }
}
