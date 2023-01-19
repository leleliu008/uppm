#include "uppm.h"

int uppm_upgrade(const char * packageName, bool verbose) {
    int ret = uppm_check_if_the_given_package_is_outdated(packageName);

    if (ret != UPPM_OK) {
        return ret;
    }

    ret = uppm_uninstall(packageName, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    return uppm_install(packageName, verbose);
}
