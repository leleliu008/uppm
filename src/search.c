#include <stdio.h>
#include <string.h>

#include "uppm.h"
#include "core/regex/regex.h"

static int package_name_callback(const char * packageName, size_t i, const void * regPattern) {
    if (regex_matched(packageName, (char*)regPattern)) {
        if (i != 0) {
            printf("\n");
        }

        return uppm_info(packageName, NULL);
    }

    return UPPM_OK;
}

int uppm_search(const char * regPattern) {
    if (regPattern == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t regPatternLength = strlen(regPattern);

    if (regPatternLength == 0) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    return uppm_list_the_available_packages(package_name_callback, regPattern);
}
