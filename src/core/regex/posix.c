#include <stdio.h>
#include <stdbool.h>
#include <regex.h>

bool regex_match(const char * content, const char * pattern) {
    if (content == NULL || pattern == NULL) {
        return false;
    }

    regex_t regex;

    if (regcomp(&regex, pattern, REG_EXTENDED) == 0) {
        regmatch_t regmatch[2];
        if (regexec(&regex, pattern, 2, regmatch, 0) == 0) {
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
