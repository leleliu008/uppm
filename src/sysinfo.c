#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "core/sysinfo.h"

#include "uppm.h"

#define COLOR_GREEN  "\033[0;32m"
#define COLOR_OFF    "\033[0m"

static void uppm_sysinfo_help() {
    if (isatty(STDOUT_FILENO)) {
        const char * str = "usage:\n\n"
        COLOR_GREEN
        "sysinfo --help\n"
        "sysinfo -h\n"
        COLOR_OFF
        "    show help of this command.\n\n"
        COLOR_GREEN
        "sysinfo --version\n"
        "sysinfo -v\n"
        COLOR_OFF
        "    show version of this command.\n\n"
        COLOR_GREEN
        "sysinfo kind\n"
        COLOR_OFF
        "    show the kind of your system.\n\n"
        COLOR_GREEN
        "sysinfo type\n"
        COLOR_OFF
        "    show the type of your system.\n\n"
        COLOR_GREEN
        "sysinfo code\n"
        COLOR_OFF
        "    show the code of your system.\n\n"
        COLOR_GREEN
        "sysinfo name\n"
        COLOR_OFF
        "    show the name of your system.\n\n"
        COLOR_GREEN
        "sysinfo vers\n"
        COLOR_OFF
        "    show the version of your system.\n\n"
        COLOR_GREEN
        "sysinfo arch\n"
        COLOR_OFF
        "    show the cpu arch of your system.\n\n"
        COLOR_GREEN
        "sysinfo ncpu\n"
        COLOR_OFF
        "    show the count of cpu core of your system.\n\n"
        COLOR_GREEN
        "sysinfo euid\n"
        COLOR_OFF
        "    show the effective user ID of your system.\n\n"
        COLOR_GREEN
        "sysinfo egid\n"
        COLOR_OFF
        "    show the effective group ID of your system.\n"
        ;

        printf("%s\n", str);
    } else {
        printf("%s\n",
            "usage:\n\n"
            "sysinfo --help\n"
            "sysinfo -h\n"
            "    show help of this command.\n\n"
            "sysinfo --version\n"
            "sysinfo -v\n"
            "    show version of this command.\n\n"
            "sysinfo kind\n"
            "    show the kind of your system.\n\n"
            "sysinfo type\n"
            "    show the type of your system.\n\n"
            "sysinfo code\n"
            "    show the code of your system.\n\n"
            "sysinfo name\n"
            "    show the name of your system.\n\n"
            "sysinfo vers\n"
            "    show the version of your system.\n\n"
            "sysinfo arch\n"
            "    show the cpu arch of your system.\n\n"
            "sysinfo ncpu\n"
            "    show the count of cpu core of your system.\n\n"
            "sysinfo euid\n"
            "    show the effective user ID of your system.\n\n"
            "sysinfo egid\n"
            "    show the effective group ID of your system.\n\n"
        );
    }
}

int uppm_sysinfo_main(int argc, char *argv[]) {
    if (argc < 3) {
        SysInfo sysinfo = {0};

        int ret = sysinfo_make(&sysinfo);

        if (ret != 0) {
            perror(NULL);
            return ret;
        }

        sysinfo_dump(&sysinfo);

        return 0;
    }

           if (strcmp(argv[2], "-h") == 0) {
        uppm_sysinfo_help();
        return 0;
    } else if (strcmp(argv[2], "--help") == 0) {
        uppm_sysinfo_help();
        return 0;
    } else if (strcmp(argv[2], "-v") == 0) {
        printf("%s\n", "1.0.1");
        return 0;
    } else if (strcmp(argv[2], "--version") == 0) {
        printf("%s\n", "1.0.1");
        return 0;
    } else if (strcmp(argv[2], "--shell") == 0) {
        SysInfo sysinfo = {0};

        int ret = sysinfo_make(&sysinfo);

        if (ret != 0) {
            perror(NULL);
            return ret;
        }

        sysinfo_dump_as_shell_script(&sysinfo);

        return 0;
    } else if (strcmp(argv[2], "kind") == 0) {
        char osKind[SYSINFO_FIELD_BUFFER_SIZE];

        int ret = sysinfo_kind(osKind, SYSINFO_FIELD_BUFFER_SIZE);

        if (ret != 0) {
            perror(NULL);
            return ret;
        }

        printf("%s\n", osKind);
        return 0;
    } else if (strcmp(argv[2], "type") == 0) {
        char osType[SYSINFO_FIELD_BUFFER_SIZE];

        int ret = sysinfo_type(osType, SYSINFO_FIELD_BUFFER_SIZE);

        if (ret != 0) {
            perror(NULL);
            return ret;
        }

        printf("%s\n", osType);
        return 0;
    } else if (strcmp(argv[2], "arch") == 0) {
        char osArch[SYSINFO_FIELD_BUFFER_SIZE];

        int ret = sysinfo_arch(osArch, SYSINFO_FIELD_BUFFER_SIZE);

        if (ret != 0) {
            perror(NULL);
            return ret;
        }

        printf("%s\n", osArch);
        return 0;
    } else if (strcmp(argv[2], "code") == 0) {
        char osCode[SYSINFO_FIELD_BUFFER_SIZE];

        int ret = sysinfo_code(osCode, SYSINFO_FIELD_BUFFER_SIZE);

        if (ret != 0) {
            perror(NULL);
            return ret;
        }

        printf("%s\n", osCode);
        return 0;
    } else if (strcmp(argv[2], "name") == 0) {
        char osName[SYSINFO_FIELD_BUFFER_SIZE];

        int ret = sysinfo_name(osName, SYSINFO_FIELD_BUFFER_SIZE);

        if (ret != 0) {
            perror(NULL);
            return ret;
        }

        printf("%s\n", osName);
        return 0;
    } else if (strcmp(argv[2], "vers") == 0) {
        char osVers[SYSINFO_FIELD_BUFFER_SIZE];

        int ret = sysinfo_vers(osVers, SYSINFO_FIELD_BUFFER_SIZE);

        if (ret != 0) {
            perror(NULL);
            return ret;
        }

        printf("%s\n", osVers);
        return 0;
    } else if (strcmp(argv[2], "ncpu") == 0) {
        int ret = sysinfo_ncpu();

        if (ret < 0) {
            perror(NULL);
            return ret;
        }

        printf("%d\n", ret);
        return 0;
    } else if (strcmp(argv[2], "libc") == 0) {
        int ret = sysinfo_libc();

        if (ret < 0) {
            perror(NULL);
            return ret;
        }

        switch(ret) {
            case LIBC_GNU:  printf("glibc\n"); break;
            case LIBC_MUSL: printf("musl\n");  break;
            default: break;
        }

        return 0;
    } else if (strcmp(argv[2], "euid") == 0) {
        printf("%d\n", geteuid());
        return 0;
    } else if (strcmp(argv[2], "egid") == 0) {
        printf("%d\n", getegid());
        return 0;
    } else {
        fprintf(stderr, "%s %s : unrecognized argument: %s\n", argv[0], argv[1], argv[2]);
        return 1;
    }
}
