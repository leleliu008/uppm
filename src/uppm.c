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

    bool verbose = false;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
            break;
        }
    }

    if (strcmp(argv[1], "env") == 0) {
        int resultCode = uppm_env(verbose);

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
        int resultCode = uppm_tree(argv[2], argc - 3, &argv[3]);

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
        UPPMDependsOutputFormat outputFormat = UPPMDependsOutputFormat_BOX;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "--format=dot") == 0) {
                outputFormat = UPPMDependsOutputFormat_DOT;
            } else if (strcmp(argv[i], "--format=box") == 0) {
                outputFormat = UPPMDependsOutputFormat_BOX;
            } else if (strcmp(argv[i], "--format=png") == 0) {
                outputFormat = UPPMDependsOutputFormat_PNG;
            } else if (strcmp(argv[i], "--format=svg") == 0) {
                outputFormat = UPPMDependsOutputFormat_SVG;
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return UPPM_ARG_IS_INVALID;
            }
        }

        int resultCode = uppm_depends(argv[2], outputFormat);

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
        int packageNameIndexes[argc];
        packageNameIndexes[0] = 0;
        packageNameIndexes[1] = 0;

        int size = 2;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                int resultCode = uppm_is_package_available(argv[i]);

                if (resultCode == UPPM_OK) {
                    packageNameIndexes[size] = i;
                    size++;
                } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                    LOG_ERROR3("package name [", argv[i], "] is not match pattern ^[A-Za-z0-9+-._]+$ ");
                } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
                    LOG_ERROR3("package [", argv[i], "] is not available.");
                }

                if (resultCode != UPPM_OK) {
                    return resultCode;
                }
            }
        }

        for (int i = 2; i < size; i++) {
            char * packageName = argv[packageNameIndexes[i]];

            int resultCode = uppm_install(packageName, verbose);

            if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
                fprintf(stderr, "package [%s] is not available.\n", packageName);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
                fprintf(stderr, "package [%s] is not installed.\n", packageName);
            } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (resultCode == UPPM_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (resultCode != UPPM_OK) {
                return resultCode;
            }
        }

        return UPPM_OK;
    }

    if (strcmp(argv[1], "uninstall") == 0) {
        int packageNameIndexes[argc];
        packageNameIndexes[0] = 0;
        packageNameIndexes[1] = 0;

        int size = 2;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                int resultCode = uppm_is_package_installed(argv[i]);

                if (resultCode == UPPM_OK) {
                    packageNameIndexes[size] = i;
                    size++;
                } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                    LOG_ERROR3("package name [", argv[i], "] is not match pattern ^[A-Za-z0-9+-._]+$ ");
                } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
                    LOG_ERROR3("package [", argv[i], "] is not installed.");
                }

                if (resultCode != UPPM_OK) {
                    return resultCode;
                }
            }
        }

        for (int i = 2; i < size; i++) {
            char * packageName = argv[packageNameIndexes[i]];

            int resultCode = uppm_uninstall(packageName, verbose);

            if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
                fprintf(stderr, "package [%s] is not available.\n", packageName);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
                fprintf(stderr, "package [%s] is not installed.\n", packageName);
            } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (resultCode == UPPM_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (resultCode != UPPM_OK) {
                return resultCode;
            }
        }

        return UPPM_OK;
    }

    if (strcmp(argv[1], "reinstall") == 0) {
        int packageNameIndexes[argc];
        packageNameIndexes[0] = 0;
        packageNameIndexes[1] = 0;

        int size = 2;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                int resultCode = uppm_is_package_available(argv[i]);

                if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                    LOG_ERROR3("package name [", argv[i], "] is not match pattern ^[A-Za-z0-9+-._]+$ ");
                } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
                    LOG_ERROR3("package [", argv[i], "] is not available.");
                }

                if (resultCode != UPPM_OK) {
                    return resultCode;
                }

                resultCode = uppm_is_package_installed(argv[i]);

                if (resultCode == UPPM_OK) {
                    packageNameIndexes[size] = i;
                    size++;
                } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                    LOG_ERROR3("package name [", argv[i], "] is not match pattern ^[A-Za-z0-9+-._]+$ ");
                } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
                    LOG_ERROR3("package [", argv[i], "] is not available.");
                }

                if (resultCode != UPPM_OK) {
                    return resultCode;
                }
            }
        }

        for (int i = 2; i < size; i++) {
            char * packageName = argv[packageNameIndexes[i]];

            int resultCode = uppm_reinstall(packageName, verbose);

            if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
                fprintf(stderr, "package [%s] is not available.\n", packageName);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
                fprintf(stderr, "package [%s] is not installed.\n", packageName);
            } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (resultCode == UPPM_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (resultCode != UPPM_OK) {
                return resultCode;
            }
        }

        return UPPM_OK;
    }

    if (strcmp(argv[1], "upgrade") == 0) {
        int packageNameIndexes[argc];
        packageNameIndexes[0] = 0;
        packageNameIndexes[1] = 0;

        int size = 2;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                int resultCode = uppm_is_package_outdated(argv[i]);

                if (resultCode == UPPM_OK) {
                    packageNameIndexes[size] = i;
                    size++;
                } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                    LOG_ERROR3("package name [", argv[i], "] is not match pattern ^[A-Za-z0-9+-._]+$ ");
                } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
                    LOG_ERROR3("package [", argv[i], "] is not available.");
                } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
                    LOG_ERROR3("package [", argv[i], "] is not installed.");
                } else if (resultCode == UPPM_PACKAGE_IS_NOT_OUTDATED) {
                    LOG_ERROR3("package [", argv[i], "] is not outdated.");
                }

                if (resultCode != UPPM_OK) {
                    return resultCode;
                }
            }
        }

        for (int i = 2; i < size; i++) {
            char * packageName = argv[packageNameIndexes[i]];

            int resultCode = uppm_upgrade(packageName, verbose);

            if (resultCode == UPPM_PACKAGE_NAME_IS_NULL) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_NAME_IS_EMPTY) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_NAME_IS_INVALID) {
                fprintf(stderr, "Usage: %s install <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern ^[A-Za-z0-9+-._]+$\n", argv[0]);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_AVAILABLE) {
                fprintf(stderr, "package [%s] is not available.\n", packageName);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_INSTALLED) {
                fprintf(stderr, "package [%s] is not installed.\n", packageName);
            } else if (resultCode == UPPM_PACKAGE_IS_NOT_OUTDATED) {
                fprintf(stderr, "package [%s] is not outdated.\n", packageName);
            } else if (resultCode == UPPM_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (resultCode == UPPM_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (resultCode == UPPM_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (resultCode != UPPM_OK) {
                return resultCode;
            }
        }

        return UPPM_OK;
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

    if (strcmp(argv[1], "formula-repo-list") == 0) {
        return uppm_formula_repo_list_printf();
    }

    if (strcmp(argv[1], "formula-repo-add") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> <FORMULA-REPO-BRANCH>\n", argv[0], argv[1]);
            return UPPM_ARG_IS_NULL;
        }

        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> <FORMULA-REPO-BRANCH>\n", argv[0], argv[1]);
            return UPPM_ARG_IS_NULL;
        }

        return uppm_formula_repo_add(argv[2], argv[3], argv[4]);
    }

    if (strcmp(argv[1], "formula-repo-del") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME>\n", argv[0], argv[1]);
            return UPPM_ARG_IS_NULL;
        }

        return uppm_formula_repo_del(argv[2]);
    }

    if (strcmp(argv[1], "integrate") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s integrate <zsh|bash|fish>\n", argv[0]);
            return UPPM_ARG_IS_NULL;
        } else if (strcmp(argv[2], "zsh") == 0) {
            return uppm_integrate_zsh_completion (NULL, verbose);
        } else if (strcmp(argv[2], "bash") == 0) {
            return uppm_integrate_bash_completion(NULL, verbose);
        } else if (strcmp(argv[2], "fish") == 0) {
            return uppm_integrate_fish_completion(NULL, verbose);
        } else {
            LOG_ERROR2("unrecognized argument: ", argv[2]);
            return UPPM_ARG_IS_INVALID;
        }
    }

    if (strcmp(argv[1], "upgrade-self") == 0) {
        return uppm_upgrade_self(verbose);
    }

    if (strcmp(argv[1], "cleanup") == 0) {
        return uppm_cleanup(verbose);
    }

    LOG_ERROR2("unrecognized action: ", argv[1]);
    return UPPM_ARG_IS_UNKNOWN;
}

int main(int argc, char* argv[]) {
    return uppm_main(argc, argv);
}
