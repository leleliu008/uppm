#include "uppm.h"

int uppm_upgrade(const char * packageName, bool verbose) {
    int resultCode = uppm_is_package_outdated(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    resultCode = uppm_uninstall(packageName, verbose);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    resultCode = uppm_install(packageName, verbose);

    return resultCode;
}
