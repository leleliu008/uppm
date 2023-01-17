#include <stdio.h>
#include <string.h>

#include "core/find-executables.h"

static void show_help(const char * programName) {
    printf("USAGE: %s <COMMAND-NAME> [-a]\n", programName);
}

int main2(int argc, char* argv[]) {
    if (argc < 2) {
        show_help(argv[0]);
        return 0;
    }

    bool findAll = false;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            findAll = true;
        } else {
            fprintf(stderr, "unrecognized argument: %s\n", argv[i]);
            show_help(argv[0]);
            return 1;
        }
    }

    char ** pathList = NULL;
    size_t  pathListSize;

    int resultCode = find_executables_in_PATH(&pathList, &pathListSize, argv[1], findAll);

    if (resultCode == 0) {
        if (pathListSize == 0) {
            return 1;
        } else {
            for (size_t i = 0; i < pathListSize; i++) {
                printf("%s\n", pathList[i]);

                free(pathList[i]);
                pathList[i] = NULL;
            }

            free(pathList);
            pathList = NULL;
        }
    }

    return resultCode;
}
