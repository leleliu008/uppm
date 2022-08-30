#include <string.h>
#include <sys/stat.h>

#include "core/http.h"
#include "core/fs.h"
#include "uppm.h"

int uppm_integrate_zsh_completion(const char * outputDir) {
    const char * url = "https://raw.githubusercontent.com/leleliu008/uppm/master/zsh-completion/_uppm";

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  zshCompletionDirLength = userHomeDirLength + 18;
    char    zshCompletionDir[zshCompletionDirLength];
    memset (zshCompletionDir, 0, zshCompletionDirLength);
    sprintf(zshCompletionDir, "%s/.uppm/zsh_completion", userHomeDir);

    if (!exists_and_is_a_directory(zshCompletionDir)) {
        if (mkdir(zshCompletionDir, S_IRWXU) != 0) {
            perror(zshCompletionDir);
            return UPPM_ERROR;
        }
    }

    size_t  zshCompletionFilePathLength = zshCompletionDirLength + 2;
    char    zshCompletionFilePath[zshCompletionFilePathLength];
    memset (zshCompletionFilePath, 0, zshCompletionFilePathLength);
    sprintf(zshCompletionFilePath, "%s/_uppm", zshCompletionDir);

    if (http_fetch_to_file(url, zshCompletionFilePath, false, false) != 0) {
        return UPPM_NETWORK_ERROR;
    }

    (void)outputDir;
    return UPPM_OK;
}

int uppm_integrate_bash_completion(const char * outputDir) {
    (void)outputDir;
    return UPPM_OK;
}

int uppm_integrate_fish_completion(const char * outputDir) {
    (void)outputDir;
    return UPPM_OK;
}
