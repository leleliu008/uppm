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
        int ret = uppm_env(verbose);

        if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "update") == 0) {
        int ret = uppm_formula_repo_list_update();

        if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "search") == 0) {
        int ret = uppm_search(argv[2]);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s search <KEYWORD>, <KEYWORD> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s search <KEYWORD>, <KEYWORD> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "info") == 0) {
        int ret = uppm_info(argv[2], argv[3]);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_ARG_IS_UNKNOWN) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], unrecognized KEY: %s\n", argv[0], argv[3]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "view") == 0) {
        bool raw = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "--raw") == 0) {
                raw = true;
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return UPPM_ERROR_ARG_IS_INVALID;
            }
        }

        int ret = uppm_formula_view(argv[2], raw);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s view <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s view <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s view <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_ARG_IS_UNKNOWN) {
            fprintf(stderr, "Usage: %s view <PACKAGE-NAME> [KEY], unrecognized KEY: %s\n", argv[0], argv[3]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "edit") == 0) {
        const char * editor = NULL;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "--editor=") == 0) {
                editor = &argv[i][9];
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return UPPM_ERROR_ARG_IS_INVALID;
            }
        }

        int ret = uppm_formula_edit(argv[2], editor);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s edit <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s edit <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s edit <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_ARG_IS_UNKNOWN) {
            fprintf(stderr, "Usage: %s edit <PACKAGE-NAME> [KEY], unrecognized KEY: %s\n", argv[0], argv[3]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "tree") == 0) {
        int ret = uppm_tree(argv[2], argc - 3, &argv[3]);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s tree <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s tree <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s tree <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
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
                return UPPM_ERROR_ARG_IS_INVALID;
            }
        }

        int ret = uppm_depends(argv[2], outputFormat);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "fetch") == 0) {
        int ret = uppm_fetch(argv[2], verbose);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s fetch <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s fetch <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s fetch <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "install") == 0) {
        if (argv[2] == NULL) {
            LOG_ERROR1("no package name is given.");
            return UPPM_ERROR;
        }

        int packageNameIndexArray[argc];
        int packageNameIndexArraySize = 0;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                int ret = uppm_check_if_the_given_package_is_available(argv[i]);

                if (ret == UPPM_OK) {
                    packageNameIndexArray[packageNameIndexArraySize] = i;
                    packageNameIndexArraySize++;
                } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                    LOG_ERROR4("package name [", argv[i], "] is not match pattern ", UPPM_PACKAGE_NAME_PATTERN);
                } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
                    LOG_ERROR3("package [", argv[i], "] is not available.");
                }

                if (ret != UPPM_OK) {
                    return ret;
                }
            }
        }

        if (packageNameIndexArraySize == 0) {
            LOG_ERROR1("no package name is given.");
            return UPPM_ERROR;
        }

        for (int i = 0; i < packageNameIndexArraySize; i++) {
            char * packageName = argv[packageNameIndexArray[i]];

            int ret = uppm_install(packageName, verbose);

            if (ret == UPPM_ERROR_ARG_IS_NULL) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
            } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0], argv[1]);
            } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], UPPM_PACKAGE_NAME_PATTERN);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
                fprintf(stderr, "package [%s] is not available.\n", packageName);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
                fprintf(stderr, "package [%s] is not installed.\n", packageName);
            } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (ret == UPPM_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (ret != UPPM_OK) {
                return ret;
            }
        }

        return UPPM_OK;
    }

    if (strcmp(argv[1], "uninstall") == 0) {
        if (argv[2] == NULL) {
            LOG_ERROR1("no package name is given.");
            return UPPM_ERROR;
        }

        int packageNameIndexArray[argc];
        int packageNameIndexArraySize = 0;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                int ret = uppm_check_if_the_given_package_is_installed(argv[i]);

                if (ret == UPPM_OK) {
                    packageNameIndexArray[packageNameIndexArraySize] = i;
                    packageNameIndexArraySize++;
                } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                    LOG_ERROR4("package name [", argv[i], "] is not match pattern ", UPPM_PACKAGE_NAME_PATTERN);
                } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
                    LOG_ERROR3("package [", argv[i], "] is not installed.");
                }

                if (ret != UPPM_OK) {
                    return ret;
                }
            }
        }

        if (packageNameIndexArraySize == 0) {
            LOG_ERROR1("no package name is given.");
            return UPPM_ERROR;
        }

        for (int i = 0; i < packageNameIndexArraySize; i++) {
            char * packageName = argv[packageNameIndexArray[i]];

            int ret = uppm_uninstall(packageName, verbose);

            if (ret == UPPM_ERROR_ARG_IS_NULL) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
            } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0], argv[1]);
            } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], UPPM_PACKAGE_NAME_PATTERN);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
                fprintf(stderr, "package [%s] is not available.\n", packageName);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
                fprintf(stderr, "package [%s] is not installed.\n", packageName);
            } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (ret == UPPM_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (ret != UPPM_OK) {
                return ret;
            }
        }

        return UPPM_OK;
    }

    if (strcmp(argv[1], "reinstall") == 0) {
        if (argv[2] == NULL) {
            LOG_ERROR1("no package name is given.");
            return UPPM_ERROR;
        }

        int packageNameIndexArray[argc];
        int packageNameIndexArraySize = 0;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                int ret = uppm_check_if_the_given_package_is_available(argv[i]);

                if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                    LOG_ERROR4("package name [", argv[i], "] is not match pattern ", UPPM_PACKAGE_NAME_PATTERN);
                } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
                    LOG_ERROR3("package [", argv[i], "] is not available.");
                }

                if (ret != UPPM_OK) {
                    return ret;
                }

                ret = uppm_check_if_the_given_package_is_installed(argv[i]);

                if (ret == UPPM_OK) {
                    packageNameIndexArray[packageNameIndexArraySize] = i;
                    packageNameIndexArraySize++;
                } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                    LOG_ERROR4("package name [", argv[i], "] is not match pattern ", UPPM_PACKAGE_NAME_PATTERN);
                } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
                    LOG_ERROR3("package [", argv[i], "] is not available.");
                }

                if (ret != UPPM_OK) {
                    return ret;
                }
            }
        }

        if (packageNameIndexArraySize == 0) {
            LOG_ERROR1("no package name is given.");
            return UPPM_ERROR;
        }

        for (int i = 0; i < packageNameIndexArraySize; i++) {
            char * packageName = argv[packageNameIndexArray[i]];

            int ret = uppm_reinstall(packageName, verbose);

            if (ret == UPPM_ERROR_ARG_IS_NULL) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
            } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0], argv[1]);
            } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], UPPM_PACKAGE_NAME_PATTERN);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
                fprintf(stderr, "package [%s] is not available.\n", packageName);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
                fprintf(stderr, "package [%s] is not installed.\n", packageName);
            } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (ret == UPPM_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (ret != UPPM_OK) {
                return ret;
            }
        }

        return UPPM_OK;
    }

    if (strcmp(argv[1], "upgrade") == 0) {
        int packageNameIndexArray[argc];
        int packageNameIndexArraySize = 0;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                int ret = uppm_check_if_the_given_package_is_outdated(argv[i]);

                if (ret == UPPM_OK) {
                    packageNameIndexArray[packageNameIndexArraySize] = i;
                    packageNameIndexArraySize++;
                } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                    LOG_ERROR4("package name [", argv[i], "] is not match pattern ", UPPM_PACKAGE_NAME_PATTERN);
                } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
                    LOG_ERROR3("package [", argv[i], "] is not available.");
                } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
                    LOG_ERROR3("package [", argv[i], "] is not installed.");
                } else if (ret == UPPM_ERROR_PACKAGE_NOT_OUTDATED) {
                    LOG_ERROR3("package [", argv[i], "] is not outdated.");
                }

                if (ret != UPPM_OK) {
                    return ret;
                }
            }
        }

        for (int i = 0; i < packageNameIndexArraySize; i++) {
            char * packageName = argv[packageNameIndexArray[i]];

            int ret = uppm_upgrade(packageName, verbose);

            if (ret == UPPM_ERROR_ARG_IS_NULL) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
            } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0], argv[1]);
            } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], UPPM_PACKAGE_NAME_PATTERN);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
                fprintf(stderr, "package [%s] is not available.\n", packageName);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
                fprintf(stderr, "package [%s] is not installed.\n", packageName);
            } else if (ret == UPPM_ERROR_PACKAGE_NOT_OUTDATED) {
                fprintf(stderr, "package [%s] is not outdated.\n", packageName);
            } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (ret == UPPM_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (ret != UPPM_OK) {
                return ret;
            }
        }

        return UPPM_OK;
    }

    if (strcmp(argv[1], "ls-available") == 0) {
        int ret = uppm_list_the_available_packages();

        if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "ls-installed") == 0) {
        int ret = uppm_list_the_installed_packages();

        if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "ls-outdated") == 0) {
        int ret = uppm_list_the_outdated__packages();

        if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "is-available") == 0) {
        int ret = uppm_check_if_the_given_package_is_available(argv[2]);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s is-available <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s is-available <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s is-available <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "is-installed") == 0) {
        int ret = uppm_check_if_the_given_package_is_installed(argv[2]);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "is-outdated") == 0) {
        int ret = uppm_check_if_the_given_package_is_outdated(argv[2]);

        if (ret == UPPM_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s is-outdated <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s is-outdated <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == UPPM_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s is-outdated <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], UPPM_PACKAGE_NAME_PATTERN);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == UPPM_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == UPPM_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == UPPM_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == UPPM_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "formula-repo-list") == 0) {
        return uppm_formula_repo_list_printf();
    }

    if (strcmp(argv[1], "formula-repo-add") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        int pinned  = 0;
        int enabled = 1;

        const char * branch = NULL;

        for (int i = 4; i < argc; i++) {
            if (strcmp(argv[i], "--pin") == 0) {
                pinned = 1;
            } else if (strcmp(argv[i], "--unpin") == 0) {
                pinned = 0;
            } else if (strcmp(argv[i], "--enable") == 0) {
                enabled = 1;
            } else if (strcmp(argv[i], "--disable") == 0) {
                enabled = 0;
            } else if (strncmp(argv[i], "--branch=", 9) == 0) {
                if (argv[i][9] == '\0') {
                    fprintf(stderr, "--branch option should have value.\n");
                    return UPPM_ERROR_ARG_IS_INVALID;
                } else {
                    branch = &argv[i][9];
                }
            } else {
                fprintf(stderr, "unrecognized option: %s\n", argv[i]);
                return UPPM_ERROR_ARG_IS_UNKNOWN;
            }
        }

        return uppm_formula_repo_add(argv[2], argv[3], branch, pinned, enabled);
    }

    if (strcmp(argv[1], "formula-repo-create") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        int pinned  = 1;
        int enabled = 1;

        char * branch = NULL;

        for (int i = 4; i < argc; i++) {
            if (strcmp(argv[i], "--pin") == 0) {
                pinned = 1;
            } else if (strcmp(argv[i], "--unpin") == 0) {
                pinned = 0;
            } else if (strcmp(argv[i], "--enable") == 0) {
                enabled = 1;
            } else if (strcmp(argv[i], "--disable") == 0) {
                enabled = 0;
            } else if (strncmp(argv[i], "--branch=", 9) == 0) {
                if (argv[i][9] == '\0') {
                    fprintf(stderr, "--branch option should have value.\n");
                    return UPPM_ERROR_ARG_IS_INVALID;
                } else {
                    branch = &argv[i][9];
                }
            } else {
                fprintf(stderr, "unrecognized option: %s\n", argv[i]);
                return UPPM_ERROR_ARG_IS_UNKNOWN;
            }
        }

        return uppm_formula_repo_create(argv[2], argv[3], branch, pinned, enabled);
    }

    if (strcmp(argv[1], "formula-repo-remove") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME>\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        return uppm_formula_repo_remove(argv[2]);
    }

    if (strcmp(argv[1], "formula-repo-update") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME>\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        return uppm_formula_repo_update(argv[2]);
    }

    if (strcmp(argv[1], "formula-repo-config") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> [--url=VALUE --branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> [--url=VALUE --branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        int pinned  = -1;
        int enabled = -1;

        const char * branch = NULL;
        const char * url = NULL;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "--pin") == 0) {
                pinned = 1;
            } else if (strcmp(argv[i], "--unpin") == 0) {
                pinned = 0;
            } else if (strcmp(argv[i], "--enable") == 0) {
                enabled = 1;
            } else if (strcmp(argv[i], "--disable") == 0) {
                enabled = 0;
            } else if (strncmp(argv[i], "--url=", 6) == 0) {
                if (argv[i][6] == '\0') {
                    fprintf(stderr, "--url option should have value.\n");
                    return UPPM_ERROR_ARG_IS_INVALID;
                } else {
                    url = &argv[i][6];
                }
            } else if (strncmp(argv[i], "--branch=", 9) == 0) {
                if (argv[i][9] == '\0') {
                    fprintf(stderr, "--branch option should have value.\n");
                    return UPPM_ERROR_ARG_IS_INVALID;
                } else {
                    branch = &argv[i][9];
                }
            } else {
                fprintf(stderr, "unrecognized option: %s\n", argv[i]);
                return UPPM_ERROR_ARG_IS_UNKNOWN;
            }
        }

        return uppm_formula_repo_config(argv[2], url, branch, pinned, enabled);
    }

    if (strcmp(argv[1], "formula-repo-info") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME>\n", argv[0], argv[1]);
            return UPPM_ERROR_ARG_IS_NULL;
        }

        return uppm_formula_repo_printf(argv[2]);
    }

    if (strcmp(argv[1], "integrate") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s integrate <zsh|bash|fish>\n", argv[0]);
            return UPPM_ERROR_ARG_IS_NULL;
        } else if (strcmp(argv[2], "zsh") == 0) {
            char * outputDirPath = NULL;

            for (int i = 3; i < argc; i++) {
                if (strncmp(argv[i], "--output-dir=", 13) == 0) {
                    if (argv[i][13] == '\0') {
                        fprintf(stderr, "--output-dir=VALUE, VALUE should be a non-empty string.\n");
                        return UPPM_ERROR_ARG_IS_INVALID;
                    } else {
                        outputDirPath = &argv[i][13];
                    }
                } else if (strcmp(argv[i], "-v") == 0) {
                    verbose = true;
                } else {
                    fprintf(stderr, "unrecognized option: %s\n", argv[i]);
                    return UPPM_ERROR_ARG_IS_UNKNOWN;
                }
            }

            return uppm_integrate_zsh_completion (outputDirPath, verbose);
        } else if (strcmp(argv[2], "bash") == 0) {
            return uppm_integrate_bash_completion(NULL, verbose);
        } else if (strcmp(argv[2], "fish") == 0) {
            return uppm_integrate_fish_completion(NULL, verbose);
        } else {
            LOG_ERROR2("unrecognized argument: ", argv[2]);
            return UPPM_ERROR_ARG_IS_INVALID;
        }
    }

    if (strcmp(argv[1], "upgrade-self") == 0) {
        return uppm_upgrade_self(verbose);
    }

    if (strcmp(argv[1], "cleanup") == 0) {
        return uppm_cleanup(verbose);
    }

    LOG_ERROR2("unrecognized action: ", argv[1]);
    return UPPM_ERROR_ARG_IS_UNKNOWN;
}

int main(int argc, char* argv[]) {
    return uppm_main(argc, argv);
}
