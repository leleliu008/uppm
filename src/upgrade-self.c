#include <string.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_upgrade_self(bool verbose) {
    const char * url = "https://raw.githubusercontent.com/leleliu008/uppm/master/zsh-completion/_uppm";

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  uppmHomeDirLength = userHomeDirLength + 7;
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    sprintf(uppmHomeDir, "%s/.uppm", userHomeDir);

    if (!exists_and_is_a_directory(uppmHomeDir)) {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    size_t  uppmUpgradeTarballFilePathLength = uppmHomeDirLength + 2;
    char    uppmUpgradeTarballFilePath[uppmUpgradeTarballFilePathLength];
    memset (uppmUpgradeTarballFilePath, 0, uppmUpgradeTarballFilePathLength);
    sprintf(uppmUpgradeTarballFilePath, "%s/uppm", uppmHomeDir);

    if (http_fetch_to_file(url, uppmUpgradeTarballFilePath, verbose, verbose) != 0) {
        return UPPM_NETWORK_ERROR;
    }

    return UPPM_OK;
}
