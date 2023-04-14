#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <git2.h>

#include "core/log.h"
#include "uppm.h"

int uppm_formula_repo_create(const char * formulaRepoName, const char * formulaRepoUrl, const char * branchName, int pinned, int enabled) {
    if (formulaRepoName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t formulaRepoNameLength = strlen(formulaRepoName);

    if (formulaRepoNameLength == 0U) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (formulaRepoUrl == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t formulaRepoUrlLength = strlen(formulaRepoUrl);

    if (formulaRepoUrlLength == 0U) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    if (branchName == NULL || strcmp(branchName, "") == 0) {
        branchName = (char*)"master";
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmHomeDirLength = userHomeDirLength + 7U;
    char     uppmHomeDir[uppmHomeDirLength];
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    if (stat(uppmHomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmHomeDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   formulaRepoRootDirLength = uppmHomeDirLength + 9U;
    char     formulaRepoRootDir[formulaRepoRootDirLength];
    snprintf(formulaRepoRootDir, formulaRepoRootDirLength, "%s/repos.d", uppmHomeDir);

    if (stat(formulaRepoRootDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", formulaRepoRootDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(formulaRepoRootDir, S_IRWXU) != 0) {
            perror(formulaRepoRootDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   formulaRepoDirLength = formulaRepoRootDirLength + formulaRepoNameLength + 2U;
    char     formulaRepoDir[formulaRepoDirLength];
    snprintf(formulaRepoDir, formulaRepoDirLength, "%s/%s", formulaRepoRootDir, formulaRepoName);

    if (stat(formulaRepoDir, &st) == 0) {
        LOG_ERROR2("formula repo already exist at: ", formulaRepoDir);
        return UPPM_ERROR_FORMULA_REPO_HAS_EXIST;
    } else {
        if (mkdir(formulaRepoDir, S_IRWXU) != 0) {
            perror(formulaRepoDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   subDirLength = formulaRepoDirLength + 9U;
    char     subDir[subDirLength];
    snprintf(subDir, subDirLength, "%s/formula", formulaRepoDir);

    if (mkdir(subDir, S_IRWXU) != 0) {
        perror(subDir);
        return UPPM_ERROR;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    git_repository   * gitRepo   = NULL;
    git_remote       * gitRemote = NULL;
    const git_error  * gitError  = NULL;

    git_libgit2_init();

    int ret = git_repository_init(&gitRepo, formulaRepoDir, false);

    if (ret != GIT_OK) {
        gitError = git_error_last();
        fprintf(stderr, "%s\n", gitError->message);
        git_repository_state_cleanup(gitRepo);
        git_repository_free(gitRepo);
        git_libgit2_shutdown();
        return abs(ret) + UPPM_ERROR_LIBGIT2_BASE;
    }

    //https://libgit2.org/libgit2/#HEAD/group/remote/git_remote_create
    ret = git_remote_create(&gitRemote, gitRepo, "origin", formulaRepoUrl);

    if (ret != GIT_OK) {
        gitError = git_error_last();
        fprintf(stderr, "%s\n", gitError->message);
        git_repository_state_cleanup(gitRepo);
        git_repository_free(gitRepo);
        git_libgit2_shutdown();
        return abs(ret) + UPPM_ERROR_LIBGIT2_BASE;
    }

    git_repository_state_cleanup(gitRepo);
    git_repository_free(gitRepo);
    git_remote_free(gitRemote);
    git_libgit2_shutdown();

    ////////////////////////////////////////////////////////////////////////////////////////

    char ts[11];
    snprintf(ts, 11, "%ld", time(NULL));

    return uppm_formula_repo_config_write(formulaRepoDir, formulaRepoUrl, branchName, pinned, enabled, ts, NULL);
}
