#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "find-executables.h"

int find_executables(ExecuablePathList ** out, const char * commandName, bool findAll) {
    if (out == NULL) {
        return FIND_EXECUTABLES_ARG_INVALID;
    }

    if ((commandName == NULL) || (strcmp(commandName, "") == 0)) {
        return FIND_EXECUTABLES_ARG_INVALID;
    }

    char * PATH = getenv("PATH");

    if ((PATH == NULL) || (strcmp(PATH, "") == 0)) {
        return FIND_EXECUTABLES_ENV_PATH_NO_VALUE;
    }

    size_t PATHLength = strlen(PATH);
    size_t PATH2Length = PATHLength + 1;
    char   PATH2[PATH2Length];
    memset(PATH2, 0, PATH2Length);
    strncpy(PATH2, PATH, PATHLength);

    size_t commandNameLength = strlen(commandName);

    size_t capcity = 0;

    ExecuablePathList * pathList = NULL;

    char * PATHItem = strtok(PATH2, ":");

    while (PATHItem != NULL) {
        struct stat st;

        if ((stat(PATHItem, &st) == 0) && S_ISDIR(st.st_mode)) {
            size_t fullPathLength = strlen(PATHItem) + commandNameLength + 2;
            char   fullPath[fullPathLength];
            snprintf(fullPath, fullPathLength, "%s/%s", PATHItem, commandName);

            if (access(fullPath, X_OK) == 0) {
                if (pathList == NULL) {
                    pathList = (ExecuablePathList*)calloc(1, sizeof(ExecuablePathList));

                    if (pathList == NULL) {
                        return FIND_EXECUTABLES_ERROR;
                    }
                }

                if (pathList->size == capcity) {
                    capcity += 2;

                    char** paths = (char**)realloc(pathList->paths, capcity * sizeof(char*));

                    if (paths == NULL) {
                        free(pathList->paths);
                        return FIND_EXECUTABLES_ERROR;
                    } else {
                        pathList->paths = paths;
                    }
                }

                char * fullPathDup = strdup(fullPath);

                if (fullPathDup == NULL) {
                    if (pathList != NULL) {
                        for (size_t i = 0; i < pathList->size; i++) {
                            free(pathList->paths[i]);
                            pathList->paths[i] = NULL;
                        }
                    }

                    free(pathList->paths);
                    free(pathList);

                    return FIND_EXECUTABLES_ERROR;
                }

                pathList->paths[pathList->size] = fullPathDup;
                pathList->size += 1;

                if (!findAll) {
                    break;
                }
            }
        }

        PATHItem = strtok(NULL, ":");
    }

    if (pathList == NULL) {
        return FIND_EXECUTABLES_NOT_FOUND;
    } else {
        (*out) = pathList;
        return FIND_EXECUTABLES_FOUND;
    }
}
