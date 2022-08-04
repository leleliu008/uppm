#include <stdio.h>
#include "uppm.h"

void uppm_show_error_message(int errorCode, const char * str) {
    switch(errorCode) {
        case UPPM_ERROR:  fprintf(stderr, "occurs error."); break;

        case UPPM_ARG_IS_NULL:  fprintf(stderr, "argument is NULL."); break;
        case UPPM_ARG_IS_EMPTY: fprintf(stderr, "argument is empty string."); break;
        case UPPM_ARG_IS_INVALID: fprintf(stderr, "argument is invalid."); break;

        case UPPM_PACKAGE_IS_NOT_AVAILABLE: fprintf(stderr, "package [%s] is not available.", str); break;
        case UPPM_PACKAGE_IS_NOT_INSTALLED: fprintf(stderr, "package [%s] is not installed.", str); break;
        case UPPM_PACKAGE_IS_NOT_OUTDATED: fprintf(stderr, "package [%s] is not outdated.", str); break;
    }
}
