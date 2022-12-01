#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/log.h"
#include "uppm.h"

int uppm_main(int argc, char* argv[]) {
    if (argc == 1) {
        uppm_help();
        return UPPM_OK;
    }

    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
        uppm_help();
        return UPPM_OK;
    }

    if ((strcmp(argv[1], "-V") == 0) || (strcmp(argv[1], "--version") == 0)) {
        printf("%s\n", UPPM_VERSION);
        return UPPM_OK;
    }

    int resultCode = uppm_init();

    if (resultCode != UPPM_OK) {
        return resultCode;
    }

    bool verbose = false;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
            break;
        }
    }

    if (strcmp(argv[1], "env") == 0) {
        int resultCode = uppm_env();

        if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "update") == 0) {
        int resultCode = uppm_formula_repo_list_update();

        if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "search") == 0) {
        int resultCode = uppm_search(argv[2]);

        if (resultCode == UPPM_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s search <KEYWORD>, <KEYWORD> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s search <KEYWORD>, <KEYWORD> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "info") == 0) {
        int resultCode = uppm_info(argv[2], argv[3]);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_INFO_UNRECOGNIZED_KEY) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], unrecognized KEY: %s\n", argv[0], argv[3]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "tree") == 0) {
        int resultCode = uppm_tree(argv[2]);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s tree <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s tree <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s tree <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "depends") == 0) {
        int resultCode = uppm_depends(argv[2]);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "fetch") == 0) {
        int resultCode = uppm_fetch(argv[2], verbose);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s fetch <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s fetch <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s fetch <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "install") == 0) {
        int resultCode = uppm_install(argv[2], verbose);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "uninstall") == 0) {
        int resultCode = uppm_uninstall(argv[2], verbose);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s uninstall <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s uninstall <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s uninstall <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "ls-available") == 0) {
        int resultCode = uppm_list_the_available_packages();

        if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "ls-installed") == 0) {
        int resultCode = uppm_list_the_installed_packages();

        if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "ls-outdated") == 0) {
        int resultCode = uppm_list_the_outdated__packages();

        if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "is-available") == 0) {
        int resultCode = uppm_is_package_available(argv[2]);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s is-available <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s is-available <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s is-available <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "is-installed") == 0) {
        int resultCode = uppm_is_package_installed(argv[2]);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "is-outdated") == 0) {
        int resultCode = uppm_is_package_outdated(argv[2]);

        if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
            fprintf(stderr, "Usage: %s is-outdated <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
            fprintf(stderr, "Usage: %s is-outdated <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
            fprintf(stderr, "Usage: %s is-outdated <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (resultCode == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return resultCode;
    }

    if (strcmp(argv[1], "formula-create") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s formula-create <FORMULA-REPO-NAME> <FORMULA-NAME>\n", argv[0]);
            return UPPM_ARG_IS_NULL;
        } else {
            return UPPM_OK;
        }
    }

    if (strcmp(argv[1], "formula-delete") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s formula-delete <FORMULA-REPO-NAME> <FORMULA-NAME>\n", argv[0]);
            return UPPM_ARG_IS_NULL;
        } else {
            return UPPM_OK;
        }
    }

    if (strcmp(argv[1], "formula-rename") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s formula-rename <FORMULA-REPO-NAME> <FORMULA-NAME>\n", argv[0]);
            return UPPM_ARG_IS_NULL;
        } else {
            return UPPM_OK;
        }
    }

    if (strcmp(argv[1], "formula-view") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s formula-view <[FORMULA-REPO-NAME/]FORMULA-NAME>\n", argv[0]);
            return UPPM_ARG_IS_NULL;
        } else {
            return uppm_formula_cat(argv[2]);
        }
    }

    if (strcmp(argv[1], "formula-edit") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s formula-edit <[FORMULA-REPO-NAME/]FORMULA-NAME>\n", argv[0]);
            return UPPM_ARG_IS_NULL;
        } else {
            return UPPM_OK;
        }
    }

    if (strcmp(argv[1], "formula-repo-list") == 0) {
        return uppm_formula_repo_list_printf();
    }

    if (strcmp(argv[1], "formula-repo-add") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-URL> <FORMULA-REPO-BRANCH>\n", argv[0], argv[1]);
            return UPPM_ARG_IS_NULL;
        }

        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-URL> <FORMULA-REPO-BRANCH>\n", argv[0], argv[1]);
            return UPPM_ARG_IS_NULL;
        }

        return uppm_formula_repo_add(argv[2], argv[3]);
    }

    if (strcmp(argv[1], "formula-repo-del") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-ID>\n", argv[0], argv[1]);
            return UPPM_ARG_IS_NULL;
        }

        return uppm_formula_repo_del(argv[2]);
    }

    if (strcmp(argv[1], "integrate") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s integrate <zsh|bash|fish>\n", argv[0]);
            return UPPM_ARG_IS_NULL;
        } else if (strcmp(argv[2], "zsh") == 0) {
            return uppm_integrate_zsh_completion(NULL);
        } else if (strcmp(argv[2], "bash") == 0) {
            return uppm_integrate_bash_completion(NULL);
        } else if (strcmp(argv[2], "fish") == 0) {
            return uppm_integrate_fish_completion(NULL);
        } else {
            fprintf(stderr, "%sunrecognized argument: %s%s\n", COLOR_RED, argv[2], COLOR_OFF);
            return 1;
        }
    }

    if (strcmp(argv[1], "cleanup") == 0) {
        return 0;
    }

    fprintf(stderr, "%sunrecognized action: %s%s\n", COLOR_RED, argv[1], COLOR_OFF);
    return UPPM_ERROR;
}
