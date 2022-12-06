#include "uppm.h"

int uppm_reinstall(const char * packageName, bool verbose) {
    int resultCode = uppm_is_package_available(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    resultCode = uppm_is_package_installed(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    resultCode = uppm_uninstall(packageName, verbose);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    return uppm_install(packageName, verbose);
}
