#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../uppm.h"

int find_executables_in_PATH(char *** listP, size_t * listSize, const char * commandName, bool findAll) {
    if (listP == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (commandName == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t commandNameLength = strlen(commandName);

    if (commandName == NULL) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    char * PATH = getenv("PATH");

    if ((PATH == NULL) || (strcmp(PATH, "") == 0)) {
        return UPPM_ERROR_ENV_PATH_NOT_SET;
    }

    size_t PATHLength = strlen(PATH);
    size_t PATH2Length = PATHLength + 1;
    char   PATH2[PATH2Length];
    memset(PATH2, 0, PATH2Length);
    strncpy(PATH2, PATH, PATHLength);

    struct stat st;

    char ** stringArrayList = NULL;
    size_t  stringArrayListCapcity = 0;
    size_t  stringArrayListSize    = 0;

    char * PATHItem = strtok(PATH2, ":");

    while (PATHItem != NULL) {
        if ((stat(PATHItem, &st) == 0) && S_ISDIR(st.st_mode)) {
            size_t fullPathLength = strlen(PATHItem) + commandNameLength + 2;
            char   fullPath[fullPathLength];
            snprintf(fullPath, fullPathLength, "%s/%s", PATHItem, commandName);

            if (access(fullPath, X_OK) == 0) {
                if (stringArrayListCapcity == stringArrayListSize) {
                    stringArrayListCapcity += 2;

                    char** paths = (char**)realloc(stringArrayList, stringArrayListCapcity * sizeof(char*));

                    if (paths == NULL) {
                        if (stringArrayList != NULL) {
                            for (size_t i = 0; i < stringArrayListSize; i++) {
                                free(stringArrayList[i]);
                                stringArrayList[i] = NULL;
                            }
                            free(stringArrayList);
                        }
                        return UPPM_ERROR_MEMORY_ALLOCATE;
                    } else {
                        stringArrayList = paths;
                    }
                }

                char * fullPathDup = strdup(fullPath);

                if (fullPathDup == NULL) {
                    if (stringArrayList != NULL) {
                        for (size_t i = 0; i < stringArrayListSize; i++) {
                            free(stringArrayList[i]);
                            stringArrayList[i] = NULL;
                        }
                        free(stringArrayList);
                    }
                    return UPPM_ERROR_MEMORY_ALLOCATE;
                }

                stringArrayList[stringArrayListSize] = fullPathDup;
                stringArrayListSize += 1;

                if (!findAll) {
                    break;
                }
            }
        }

        PATHItem = strtok(NULL, ":");
    }

    (*listP) =    stringArrayList;
    (*listSize) = stringArrayListSize;

    return UPPM_OK;
}
