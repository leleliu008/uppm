#include "uppm.h"

int uppm_reinstall(const char * packageName, bool verbose) {
    int resultCode = uppm_check_if_the_given_package_is_available(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    resultCode = uppm_check_if_the_given_package_is_installed(packageName);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    resultCode = uppm_uninstall(packageName, verbose);

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    return uppm_install(packageName, verbose);
}
