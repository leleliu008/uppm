#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include "util.h"

bool regex_match(const char * contentStr, const char * regexStr) {
    if (contentStr == NULL) {
        return false;
    }

    regex_t regex;

    if (regcomp(&regex, regexStr, REG_EXTENDED) == 0) {
        regmatch_t regmatch[2];
        if (regexec(&regex, contentStr, 2, regmatch, 0) == 0) {
            //printf("regmatch[0].rm_so=%d\n", regmatch[0].rm_so);
            //printf("regmatch[0].rm_eo=%d\n", regmatch[0].rm_eo);
            if (regmatch[0].rm_so >= 0 && regmatch[0].rm_eo > regmatch[0].rm_so) {
                regfree(&regex);
                return true;
            }
        }
    }

    regfree(&regex);
    return false;
}

int get_file_type_id_from_url(const char * url) {
    if (regex_match(url, ".*\\.zip$")) {
        return 1;
    } else if (regex_match(url, ".*\\.tar\\.gz$")) {
        return 2;
    } else if (regex_match(url, ".*\\.tar\\.xz$")) {
        return 3;
    } else if (regex_match(url, ".*\\.tar\\.lz$")) {
        return 4;
    } else if (regex_match(url, ".*\\.tar\\.bz2$")) {
        return 5;
    } else if (regex_match(url, ".*\\.tgz$")) {
        return 2;
    } else if (regex_match(url, ".*\\.txz$")) {
        return 3;
    } else if (regex_match(url, ".*\\.tlz$")) {
        return 4;
    } else {
        return 0;
    }
}
