#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "core/rm-r.h"
#include "core/log.h"

#include "uppm.h"

int uppm_generate_url_transform_sample() {
    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmRunDIRLength = uppmHomeDIRLength + 5U;
    char     uppmRunDIR[uppmRunDIRLength];
    snprintf(uppmRunDIR, uppmRunDIRLength, "%s/run", uppmHomeDIR);

    if (stat(uppmRunDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "%s was expected to be a directory, but it was not.\n", uppmRunDIR);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmRunDIR, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                perror(uppmRunDIR);
                return UPPM_ERROR;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    size_t   sessionDIRLength = uppmRunDIRLength + 20U;
    char     sessionDIR[sessionDIRLength];
    snprintf(sessionDIR, sessionDIRLength, "%s/%d", uppmRunDIR, getpid());

    if (stat(sessionDIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (rm_r(sessionDIR, false) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
        } else {
            fprintf(stderr, "%s was expected to be a directory, but it was not.\n", sessionDIR);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(sessionDIR, S_IRWXU) != 0) {
            perror(sessionDIR);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    size_t   tmpFilePathLength = sessionDIRLength + 22U;
    char     tmpFilePath[tmpFilePathLength];
    snprintf(tmpFilePath, tmpFilePathLength, "%s/url-transform.sample", sessionDIR);

    int fd = open(tmpFilePath, O_CREAT | O_TRUNC | O_WRONLY, 0666);

    if (fd == -1) {
        perror(tmpFilePath);
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
        "    '') printf '%s\\n' \"$0 <URL>, <URL> is unspecified.\" >&2 ; exit 1 ;;\n"
        "    *)  printf '%s\\n' \"$1\"\n"
        "esac";

    size_t pSize = strlen(p);

    ssize_t writeSize = write(fd, p, pSize);

    if (writeSize == -1) {
        perror(tmpFilePath);
        close(fd);
        return UPPM_ERROR;
    }

    close(fd);

    if ((size_t)writeSize != pSize) {
        fprintf(stderr, "not fully written to %s\n", tmpFilePath);
        return UPPM_ERROR;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    if (chmod(tmpFilePath, S_IRWXU) != 0) {
        perror(tmpFilePath);
        return UPPM_ERROR;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    size_t   outFilePathLength = uppmHomeDIRLength + 22U;
    char     outFilePath[outFilePathLength];
    snprintf(outFilePath, outFilePathLength, "%s/url-transform.sample", uppmHomeDIR);

    if (rename(tmpFilePath, outFilePath) != 0) {
        if (errno == EXDEV) {
            ret = uppm_copy_file(tmpFilePath, outFilePath);

            if (ret != UPPM_OK) {
                return ret;
            }
        } else {
            perror(tmpFilePath);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    fprintf(stderr, "%surl-transform sample has been written into %s%s\n\n", COLOR_GREEN, outFilePath, COLOR_OFF);

    outFilePath[outFilePathLength - 9] = '\0';

    fprintf(stderr, "%sYou can rename url-transform.sample to url-transform then edit it to meet your needs.\n\nTo apply this, you should run 'export UPPM_URL_TRANSFORM=%s' in your terminal.\n%s", COLOR_GREEN, outFilePath, COLOR_OFF);

    if (rm_r(sessionDIR, false) != 0) {
        perror(sessionDIR);
        return UPPM_ERROR;
    }

    return UPPM_OK;
}
