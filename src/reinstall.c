#include "uppm.h"

int uppm_reinstall(const char * packageName, const bool verbose) {
    return uppm_install(packageName, verbose, true);
}
