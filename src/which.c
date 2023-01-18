#include <stdio.h>
#include <string.h>

#include "core/exe.h"

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

    int resultCode = exe_search(argv[1], &pathList, &pathListSize, findAll);

    if (resultCode == 0) {
        for (size_t i = 0; i < pathListSize; i++) {
            printf("%s\n", pathList[i]);

            free(pathList[i]);
            pathList[i] = NULL;
        }

        free(pathList);
        pathList = NULL;
    }

    return resultCode;
}
