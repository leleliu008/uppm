#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "core/fs.h"
#include "uppm.h"

static bool check_os_kind_arch_is_supported(const char * osKind, const char * osArch) {
    if (strcmp(osArch, "x86_64") == 0) {
        return true;
    } else if (strcmp(osArch, "arm64") == 0) {
        return true;
    } else {
        fprintf(stderr, "%s/%s is not supported yet.", osKind, osArch);
        return false;
    }
}

UPPMFormulaRepo* uppm_formula_repo_default_new(char * userHomeDir, size_t userHomeDirLength) {
    struct utsname uts;

    if (uname(&uts) < 0) {
        perror("uname() error");
        return NULL;
    }

    const char * osKind = uts.sysname;
    const char * osArch = uts.machine;

    if (strcmp(osKind, "Linux") == 0) {
        osKind = "linux";
    } else if (strcmp(osKind, "Darwin") == 0) {
        osKind = "macos";
    } else if (strcmp(osKind, "FreeBSD") == 0) {
        osKind = "freebsd";
    } else if (strcmp(osKind, "OpenBSD") == 0) {
        osKind = "openbsd";
    } else if (strcmp(osKind, "NetBSD") == 0) {
        osKind = "netbsd";
    }

    if (strcmp(osArch, "amd64") == 0) {
        osArch = "x86_64";
    } else if (strcmp(osArch, "aarch64") == 0) {
        osArch = "arm64";
    }

    char *  formulaRepoPath = (char*)calloc(userHomeDirLength + 23, sizeof(char));
    sprintf(formulaRepoPath, "%s/.uppm/repos.d/offical", userHomeDir);

    char *  formulaRepoUrl = (char*)calloc(strlen(osKind) + strlen(osArch) + 60, sizeof(char));
    sprintf(formulaRepoUrl, "https://github.com/leleliu008/uppm-formula-repository-%s-%s.git", osKind, osArch);

    UPPMFormulaRepo * formulaRepo = (UPPMFormulaRepo*)calloc(1, sizeof(UPPMFormulaRepo));
    formulaRepo->name = strdup("offical");
    formulaRepo->url  = formulaRepoUrl;
    formulaRepo->path = formulaRepoPath;

    return formulaRepo;
}

int uppm_formula_repo_list_new(UPPMFormulaRepoList * * out) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL || strcmp(userHomeDir, "") == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    size_t  formulaRepoConfigFilePathLength = userHomeDirLength + 13;
    char    formulaRepoConfigFilePath[formulaRepoConfigFilePathLength];
    memset (formulaRepoConfigFilePath, 0, formulaRepoConfigFilePathLength);
    sprintf(formulaRepoConfigFilePath, "%s/.uppm/repos", userHomeDir);

    if (exists_and_is_a_regular_file(formulaRepoConfigFilePath)) {
        FILE * formulaRepoConfigFile = fopen(formulaRepoConfigFilePath, "r");

        if (formulaRepoConfigFile == NULL) {
            perror(formulaRepoConfigFilePath);
            return UPPM_REPOS_CONFIG_READ_ERROR;
        }

        UPPMFormulaRepoList * formulaRepoList = NULL;

        size_t capacity = 3;

        char line[300];
        while(fgets(line, 300, formulaRepoConfigFile)) {
            char* repoName = NULL;
            char* repoUrl  = NULL;

            repoName = strtok(line, "=");
            repoUrl  = strtok(NULL, "=");

            if ((repoName == NULL) || (repoUrl == NULL)) {
                perror("~/.uppm/repos config file syntax error.");
                fclose(formulaRepoConfigFile);
                uppm_formula_repo_list_free(formulaRepoList);
                return UPPM_REPOS_CONFIG_READ_ERROR;
            }

            size_t lineLength = strlen(repoUrl);
            size_t lastIndex  = lineLength - 1;

            if (repoUrl[lastIndex] == '\n') {
                repoUrl[lastIndex] =  '\0';
            }

            char * formulaRepoPath = (char*)calloc(userHomeDirLength + 16 + strlen(repoName), sizeof(char));
            sprintf(formulaRepoPath, "%s/.uppm/repos.d/%s", userHomeDir, repoName);

            if (formulaRepoList == NULL) {
                formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));
                formulaRepoList->repos = (UPPMFormulaRepo**)calloc(capacity, sizeof(UPPMFormulaRepo*));
                formulaRepoList->repos[0] = uppm_formula_repo_default_new(userHomeDir, userHomeDirLength);
                formulaRepoList->size     = 1;
            }

            if (strcmp(repoName, "offical") == 0) {
                free(formulaRepoList->repos[0]->url);
                formulaRepoList->repos[0]->url = strdup(repoUrl);
            } else {
                UPPMFormulaRepo * formulaRepo = (UPPMFormulaRepo*)calloc(1, sizeof(UPPMFormulaRepo));
                formulaRepo->path = formulaRepoPath;
                formulaRepo->name = strdup(repoName);
                formulaRepo->url  = strdup(repoUrl);

                if (formulaRepoList->size == capacity) {
                    capacity += 5;
                    formulaRepoList->repos = (UPPMFormulaRepo**)realloc(formulaRepoList->repos, capacity * sizeof(UPPMFormulaRepo*));
                }

                formulaRepoList->repos[formulaRepoList->size] = formulaRepo;
                formulaRepoList->size += 1;
            }
        }

        fclose(formulaRepoConfigFile);

        (*out) = formulaRepoList;
    } else {
        UPPMFormulaRepoList * formulaRepoList = (UPPMFormulaRepoList*)calloc(1, sizeof(UPPMFormulaRepoList));
        formulaRepoList->repos = (UPPMFormulaRepo**)calloc(1, sizeof(UPPMFormulaRepo*));
        formulaRepoList->repos[0] = uppm_formula_repo_default_new(userHomeDir, userHomeDirLength);
        formulaRepoList->size     = 1;

        (*out) = formulaRepoList;
    }

    return UPPM_OK;
}

void uppm_formula_repo_list_free(UPPMFormulaRepoList * formulaRepoList) {
    if (formulaRepoList == NULL) {
        return;
    }

    if (formulaRepoList->repos == NULL) {
        free(formulaRepoList);
        return;
    }

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        UPPMFormulaRepo * formulaRepo = formulaRepoList->repos[i];

        free(formulaRepo->name);
        free(formulaRepo->path);
        free(formulaRepo->url);

        formulaRepo->name = NULL;
        formulaRepo->path = NULL;
        formulaRepo->url  = NULL;

        free(formulaRepoList->repos[i]);
        formulaRepoList->repos[i] = NULL;
    }

    free(formulaRepoList->repos);
    formulaRepoList->repos = NULL;

    free(formulaRepoList);
}
