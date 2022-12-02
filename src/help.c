#include <unistd.h>
#include "core/log.h"

int uppm_help() {
    if (isatty(STDOUT_FILENO)) {
        const char * str = ""
        COLOR_GREEN
        "uppm (Universal Prebuild Package Manager)\n\n"
        "uppm <ACTION> [ARGUMENT...]\n\n"
        "uppm --help\n"
        "uppm -h\n"
        COLOR_OFF
        "    show help of this command.\n\n"
        COLOR_GREEN
        "uppm --version\n"
        "uppm -V\n"
        COLOR_OFF
        "    show version of this command.\n\n"
        COLOR_GREEN
        "uppm env\n"
        COLOR_OFF
        "    show current machine os info.\n\n"
        COLOR_GREEN
        "uppm update\n"
        COLOR_OFF
        "    update the formula repositories.\n\n"
        COLOR_GREEN
        "uppm search  <keyword>\n"
        COLOR_OFF
        "    search packages whose name match the given keyword.\n\n"
        COLOR_GREEN
        "uppm info    <PACKAGE-NAME|@all>\n"
        COLOR_OFF
        "    show information of the given package.\n\n"
        COLOR_GREEN
        "ppkg tree <PACKAGE-NAME> [--dirsfirst | -L N]\n"
        COLOR_OFF
        "    list files of the given installed package in a tree-like format.\n\n"
        COLOR_GREEN
        "uppm depends <PACKAGE-NAME>\n"
        COLOR_OFF
        "    show packages that are depended by the given package.\n\n"
        COLOR_GREEN
        "uppm fetch   <PACKAGE-NAME|@all>\n"
        COLOR_OFF
        "    download the prebuild binary archive file of the given package to the local cache.\n\n"
        COLOR_GREEN
        "uppm   install <PACKAGE-NAME>\n"
        COLOR_OFF
        "    install the given packages.\n\n"
        COLOR_GREEN
        "uppm   upgrade <PACKAGE-NAME>\n"
        COLOR_OFF
        "    upgrade the given packages or all outdated packages.\n\n"
        COLOR_GREEN
        "uppm reinstall <PACKAGE-NAME>\n"
        COLOR_OFF
        "    reinstall the given packages.\n\n"
        COLOR_GREEN
        "uppm uninstall <PACKAGE-NAME>\n"
        COLOR_OFF
        "    uninstall the given packages.\n\n"
        COLOR_GREEN
        "uppm ls-available\n"
        COLOR_OFF
        "    list the available packages.\n\n"
        COLOR_GREEN
        "uppm ls-installed\n"
        COLOR_OFF
        "    list the installed packages.\n\n"
        COLOR_GREEN
        "uppm ls-outdated\n"
        COLOR_OFF
        "    list the outdated  packages.\n\n"
        COLOR_GREEN
        "uppm is-available <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is available.\n\n"
        COLOR_GREEN
        "uppm is-installed <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is installed.\n\n"
        COLOR_GREEN
        "uppm is-outdated  <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is outdated.\n\n"
        COLOR_GREEN
        "uppm cleanup\n"
        COLOR_OFF
        "    cleanup the unused cache.\n\n"
        COLOR_GREEN
        "uppm formula-repo-list\n"
        COLOR_OFF
        "    list formula repositories.";

        printf("%s\n", str);
    } else {
        const char * str = ""
        "uppm (Universal Prebuild Package Manager)\n\n"
        "uppm <ACTION> [ARGUMENT...]\n\n"
        "uppm --help\n"
        "uppm -h\n"
        "    show help of this command.\n\n"
        "uppm --version\n"
        "uppm -V\n"
        "    show version of this command.\n\n"
        "uppm env\n"
        "    show current machine os info.\n\n"
        "uppm update\n"
        "    update the formula repositories.\n\n"
        "uppm search  <keyword>\n"
        "    search packages whose name match the given keyword.\n\n"
        "uppm info    <PACKAGE-NAME|@all>\n"
        "    show information of the given package.\n\n"
        "ppkg tree <PACKAGE-NAME> [--dirsfirst | -L N]\n"
        "    list files of the given installed package in a tree-like format.\n\n"
        "uppm depends <PACKAGE-NAME>\n"
        "    show packages that are depended by the given package.\n\n"
        "uppm fetch   <PACKAGE-NAME|@all>\n"
        "    download the prebuild binary archive file of the given package to the local cache.\n\n"
        "uppm   install <PACKAGE-NAME>\n"
        "    install the given packages.\n\n"
        "uppm   upgrade <PACKAGE-NAME>\n"
        "    upgrade the given packages or all outdated packages.\n\n"
        "uppm reinstall <PACKAGE-NAME>\n"
        "    reinstall the given packages.\n\n"
        "uppm uninstall <PACKAGE-NAME>\n"
        "    uninstall the given packages.\n\n"
        "uppm ls-available\n"
        "    list the available packages.\n\n"
        "uppm ls-installed\n"
        "    list the installed packages.\n\n"
        "uppm ls-outdated\n"
        "    list the outdated  packages.\n\n"
        "uppm is-available <PACKAGE-NAME>\n"
        "    check if the given package is available.\n\n"
        "uppm is-installed <PACKAGE-NAME>\n"
        "    check if the given package is installed.\n\n"
        "uppm is-outdated  <PACKAGE-NAME>\n"
        "    check if the given package is outdated.\n\n"
        "uppm cleanup\n"
        "    cleanup the unused cache.\n\n"
        "uppm formula-repo-list\n"
        "    list formula repositories.";

        printf("%s\n", str);
    }

    return 0;
}
