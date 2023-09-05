#include "uppm.h"

int uppm_reinstall(const char * packageName, bool verbose) {
    return uppm_install(packageName, verbose, true);
}
