#include <regex.h>
#include "regex/regex.h"
#include "util.h"

int get_file_type_id_from_url(const char * url) {
    if (regex_matched(url, ".*\\.zip$")) {
        return 1;
    } else if (regex_matched(url, ".*\\.tar\\.gz$")) {
        return 2;
    } else if (regex_matched(url, ".*\\.tar\\.xz$")) {
        return 3;
    } else if (regex_matched(url, ".*\\.tar\\.lz$")) {
        return 4;
    } else if (regex_matched(url, ".*\\.tar\\.bz2$")) {
        return 5;
    } else if (regex_matched(url, ".*\\.tgz$")) {
        return 2;
    } else if (regex_matched(url, ".*\\.txz$")) {
        return 3;
    } else if (regex_matched(url, ".*\\.tlz$")) {
        return 4;
    } else {
        return 0;
    }
}
