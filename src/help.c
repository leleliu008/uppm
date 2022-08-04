#include "core/log.h"

int uppm_help() {
    printf("%s\n",
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
        "uppm ls-available-packages\n"
        COLOR_OFF
        "    list the available packages.\n\n"
        COLOR_GREEN
        "uppm ls-installed-packages\n"
        COLOR_OFF
        "    list the installed packages.\n\n"
        COLOR_GREEN
        "uppm ls-outdated-packages\n"
        COLOR_OFF
        "    list the outdated  packages.\n\n"
        COLOR_GREEN
        "uppm is-package-available <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is available.\n\n"
        COLOR_GREEN
        "uppm is-package-installed <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is installed.\n\n"
        COLOR_GREEN
        "uppm is-package-outdated  <PACKAGE-NAME>\n"
        COLOR_OFF
        "    check if the given package is outdated.\n\n"
        COLOR_GREEN
        "uppm cleanup\n"
        COLOR_OFF
        "    cleanup the unused cache.\n\n"
        COLOR_GREEN
        "uppm formula-repo [list]\n"
        COLOR_OFF
        "    list formula repositories."
    );

    return 0;
}
