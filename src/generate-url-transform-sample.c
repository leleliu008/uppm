#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/log.h"

#include "uppm.h"

int uppm_generate_url_transform_sample() {
    char   uppmHomeDir[256];
    size_t uppmHomeDirLength;

    int ret = uppm_home_dir(uppmHomeDir, 256, &uppmHomeDirLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////

    size_t   urlTransformSampleFilePathLength = uppmHomeDirLength + 22U;
    char     urlTransformSampleFilePath[urlTransformSampleFilePathLength];
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
        fprintf(stderr, "%surl-transform sample has been written into %s%s\n\n", COLOR_GREEN, urlTransformSampleFilePath, COLOR_OFF);

        urlTransformSampleFilePath[urlTransformSampleFilePathLength - 9] = '\0';

        fprintf(stderr, "%sYou can rename url-transform.sample to url-transform then edit it to meet your needs.\n\nTo apply this, you should run 'export UPPM_URL_TRANSFORM=%s' in your terminal.\n%s", COLOR_GREEN, urlTransformSampleFilePath, COLOR_OFF);
        return UPPM_OK;
    } else {
        perror(urlTransformSampleFilePath);
        return UPPM_ERROR;
    }
}
