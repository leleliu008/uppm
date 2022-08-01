#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uppm.h"
#include "log.h"

int uppm_main(int argc, char* argv[]) {
    if (argc == 1) {
        uppm_help();
        return 0;
    }

    const char * action = argv[1];

    if ((strcmp(action, "-h") == 0) || (strcmp(action, "--help") == 0)) {
        uppm_help();
        return 0;
    }

    if ((strcmp(action, "-V") == 0) || (strcmp(action, "--version") == 0)) {
        printf("%s\n", UPPM_VERSION);
        return 0;
    }

    if (uppm_init() != 0) {
        return 1;
    }

    if (strcmp(action, "env") == 0) {
        printf("NATIVE_OS_KIND = %s\n", getenv("NATIVE_OS_KIND"));
        printf("NATIVE_OS_NAME = %s\n", getenv("NATIVE_OS_NAME"));
        printf("NATIVE_OS_ARCH = %s\n", getenv("NATIVE_OS_ARCH"));
        printf("\n");
        printf("UPPM_HOME      = %s\n", getenv("UPPM_HOME"));
        printf("UPPM_VERSION   = %s\n", getenv("UPPM_VERSION"));
        return 0;
    }

    if (strcmp(action, "update") == 0) {
        return uppm_formula_repo_list_update();
    }

    if (strcmp(action, "search") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s search <KEYWORD>, <KEYWORD> is not given.\n", argv[0]);
            return 1;
        } else {
            return uppm_search(argv[2]);
        }
    }

    if (strcmp(action, "info") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            return 1;
        } else {
            return uppm_info(argv[2]);
        }
    }

    if (strcmp(action, "depends") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            return 1;
        } else {
            return uppm_depends(argv[2]);
        }
    }

    if (strcmp(action, "fetch") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s fetch <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            return 1;
        } else {
            return uppm_fetch(argv[2]);
        }
    }

    if (strcmp(action, "install") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            return 1;
        } else {
            return uppm_install(argv[2]);
        }
    }

    if (strcmp(action, "uninstall") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s uninstall <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            return 1;
        } else {
            return uppm_uninstall(argv[2]);
        }
    }

    if (strcmp(action, "cleanup") == 0) {
        return 0;
    }

    if (strcmp(action, "ls-available-packages") == 0) {
        return uppm_list_the_available_packages();
    }

    if (strcmp(action, "ls-installed-packages") == 0) {
        return uppm_list_the_installed_packages();
    }

    if (strcmp(action, "ls-outdated-packages") == 0) {
        return uppm_list_the_outdated__packages();
    }

    if (strcmp(action, "is-package-available") == 0) {
        if (argc == 2) {
            fprintf(stderr, "Usage: %s is-package-available <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            return UPPM_ERROR;
        } else if (argc == 3) {
            return uppm_is_package_available(argv[2]);
        } else {
            fprintf(stderr, "Usage: %s is-package-available <PACKAGE-NAME>, too many arguments.\n", argv[0]);
            return UPPM_ERROR;
        }
    }

    if (strcmp(action, "is-package-installed") == 0) {
        if (argc == 2) {
            fprintf(stderr, "Usage: %s is-package-installed <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            return UPPM_ERROR;
        } else if (argc == 3) {
            return uppm_is_package_installed(argv[2]);
        } else {
            fprintf(stderr, "Usage: %s is-package-installed <PACKAGE-NAME>, too many arguments.\n", argv[0]);
            return UPPM_ERROR;
        }
    }

    if (strcmp(action, "is-package-outdated") == 0) {
        if (argc == 2) {
            fprintf(stderr, "Usage: %s is-package-outdated <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            return UPPM_ERROR;
        } else if (argc == 3) {
            return uppm_is_package_outdated(argv[2]);
        } else {
            fprintf(stderr, "Usage: %s is-package-outdated <PACKAGE-NAME>, too many arguments.\n", argv[0]);
            return UPPM_ERROR;
        }
    }

    if (strcmp(action, "formula-repo") == 0) {
        if (argv[2] == NULL || strcmp(argv[2], "list") == 0) {
            return uppm_formula_repo_list_printf();
        } else if (strcmp(argv[2], "update") == 0) {
            return uppm_formula_repo_list_update();
        } else {
            fprintf(stderr, "%sunrecognized action: %s%s\n", COLOR_RED, argv[2], COLOR_OFF);
            return 1;
        }
    }

    fprintf(stderr, "%sunrecognized action: %s%s\n", COLOR_RED, action, COLOR_OFF);
    return 1;
}
