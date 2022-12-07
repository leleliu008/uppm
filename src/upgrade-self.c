#include <string.h>
#include <sys/stat.h>
#include <libgen.h>

#include "core/http.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_upgrade_self(bool verbose) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    if (strcmp(userHomeDir, "") == 0) {
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

    const char * url = "https://raw.githubusercontent.com/leleliu008/uppm/master/zsh-completion/_uppm";

    size_t  urlLength = strlen(url);
    size_t  urlCopyLength = urlLength + 1;
    char    urlCopy[urlCopyLength];
    memset (urlCopy, 0, urlCopyLength);
    strncpy(urlCopy, url, urlLength);

    const char * filename = basename(urlCopy);

    size_t  filepathLength = uppmHomeDirLength + 2;
    char    filepath[filepathLength];
    memset (filepath, 0, filepathLength);
    sprintf(filepath, "%s/%s", uppmHomeDir, filename);

    if (http_fetch_to_file(url, filepath, verbose, verbose) != 0) {
        return UPPM_NETWORK_ERROR;
    }

    return UPPM_OK;
}
