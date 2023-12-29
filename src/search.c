#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "core/regex/regex.h"

#include "uppm.h"

static size_t j = 0U;

static int package_name_filter(const char * packageName, const size_t i, const bool verbose, const void * regPattern) {
    if (regex_matched(packageName, (char*)regPattern) == 0) {
        if (verbose) {
            if (j != 0U) {
                printf("\n");
            }

            j++;

            return uppm_available_info(packageName, NULL);
        } else {
            puts(packageName);
            return UPPM_OK;
        }
    } else {
        if (errno == 0) {
            return UPPM_OK;
        } else {
            perror(NULL);
            return UPPM_ERROR;
        }
    }
}

int uppm_search(const char * regPattern, const bool verbose) {
    if (regPattern == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (regPattern[0] == '\0') {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    return uppm_list_the_available_packages(verbose, package_name_filter, regPattern);
}
