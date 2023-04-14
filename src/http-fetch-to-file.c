#include <stdio.h>
#include <string.h>

#include "core/url-transform.h"
#include "core/http.h"

#include "uppm.h"

int uppm_http_fetch_to_file(const char * url, const char * outputFilePath, bool verbose, bool showProgress) {
    const char * urlTransformCommandPath = getenv("UPPM_URL_TRANSFORM");

    if (urlTransformCommandPath == NULL || strcmp(urlTransformCommandPath, "") == 0) {
        int ret = http_fetch_to_file(url, outputFilePath, verbose, showProgress);

        if (ret == -1) {
            perror(outputFilePath);
            return UPPM_ERROR;
        }

        if (ret > 0) {
            return UPPM_ERROR_NETWORK_BASE + ret;
        }

        return UPPM_OK;
    } else {
        if (verbose) {
            fprintf(stderr, "\nyou have set UPPM_URL_TRANSFORM=%s\n", urlTransformCommandPath);
            fprintf(stderr, "transform from: %s\n", url);
        }

        char   transformedUrl[1025] = {0};
        size_t transformedUrlLength = 0;

        if (url_transform(urlTransformCommandPath, url, transformedUrl, 1024, &transformedUrlLength, true) != 0) {
            perror(urlTransformCommandPath);
            return UPPM_ERROR;
        }

        if (transformedUrlLength == 0U) {
            if (verbose) {
                fprintf(stderr, "a new url was expected to be output, but it was not.\n");
            }
            return UPPM_ERROR;
        }

        if (verbose) {
            fprintf(stderr, "transform   to: %s\n", transformedUrl);
        }

        int ret = http_fetch_to_file(transformedUrl, outputFilePath, verbose, showProgress);

        if (ret == -1) {
            perror(outputFilePath);
            return UPPM_ERROR;
        }

        if (ret > 0) {
            return UPPM_ERROR_NETWORK_BASE + ret;
        }

        return UPPM_OK;
    }
}
