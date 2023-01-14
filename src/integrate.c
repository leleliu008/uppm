#include <string.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_integrate_zsh_completion(const char * outputDir, bool verbose) {
    const char * url = "https://raw.githubusercontent.com/leleliu008/uppm/master/zsh-completion/_uppm";

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    size_t  uppmHomeDirLength = userHomeDirLength + 7;
    char    uppmHomeDir[uppmHomeDirLength];
    memset (uppmHomeDir, 0, uppmHomeDirLength);
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    if (!exists_and_is_a_directory(uppmHomeDir)) {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t  zshCompletionDirLength = uppmHomeDirLength + 16;
    char    zshCompletionDir[zshCompletionDirLength];
    memset (zshCompletionDir, 0, zshCompletionDirLength);
    snprintf(zshCompletionDir, zshCompletionDirLength, "%s/zsh_completion", uppmHomeDir);

    if (!exists_and_is_a_directory(zshCompletionDir)) {
        if (mkdir(zshCompletionDir, S_IRWXU) != 0) {
            perror(zshCompletionDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t  zshCompletionFilePathLength = zshCompletionDirLength + 7;
    char    zshCompletionFilePath[zshCompletionFilePathLength];
    memset (zshCompletionFilePath, 0, zshCompletionFilePathLength);
    snprintf(zshCompletionFilePath, zshCompletionFilePathLength, "%s/_uppm", zshCompletionDir);

    if (http_fetch_to_file(url, zshCompletionFilePath, verbose, verbose) != 0) {
        return UPPM_NETWORK_ERROR;
    }

    (void)outputDir;
    return UPPM_OK;
}

int uppm_integrate_bash_completion(const char * outputDir, bool verbose) {
    (void)outputDir;
    (void)verbose;
    return UPPM_OK;
}

int uppm_integrate_fish_completion(const char * outputDir, bool verbose) {
    (void)outputDir;
    (void)verbose;
    return UPPM_OK;
}
