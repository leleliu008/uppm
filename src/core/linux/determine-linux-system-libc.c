#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <elf.h>

#define ERROR_STAT               100
#define ERROR_OPEN               101
#define ERROR_SEEK               102
#define ERROR_READ               103
#define ERROR_NOT_FULLY_READ     104

#define ERROR_NOT_ELF_FILE       120
#define ERROR_BROKEN_ELF_FILE    130
#define ERROR_NOT_DETERMINED     200

#define LIBC_GNU     1
#define LIBC_MUSL    2

static int handle_elf32(const int fd, const char * const fp) {
    Elf32_Ehdr ehdr;

    int ret = read(fd, &ehdr, sizeof(Elf32_Ehdr));

    if (ret == -1) {
        perror(fp);
        return ERROR_READ;
    }

    if (ret != sizeof(Elf32_Ehdr)) {
        perror(fp);
        return ERROR_NOT_FULLY_READ;
    }

    ///////////////////////////////////////////////////////////

    Elf32_Phdr phdr;

    for (unsigned int i = 1; i < ehdr.e_phnum; i++) {
        ret = pread(fd, &phdr, sizeof(Elf32_Phdr), ehdr.e_phoff + i * ehdr.e_phentsize);

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != sizeof(Elf32_Phdr)) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        if (phdr.p_type != PT_INTERP) {
            continue;
        }

        char p[phdr.p_filesz];

        ret = pread(fd, p, phdr.p_filesz, phdr.p_offset);

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != phdr.p_filesz) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        fprintf(stderr, "%s interpreter : %s\n", fp, p);

        /*
         * default dynamic loaders for glibc:
         *
         * x86_64     /lib64/ld-linux-x86-64.so.2
         * aarch64    /lib/ld-linux-aarch64.so.1
         * riscv64    /lib/ld-linux-riscv64-lp64d.so.1
         * armhf      /lib/ld-linux-armhf.so.3
         * ppc64le    /lib/powerpc64le-linux-gnu/ld64.so.2
         * ppc64le    /lib64/ld64.so.2
         * s390x      /lib/ld64.so.1
         */

        /*
         * default dynamic loaders for musl:
         *
         * armhf       /lib/ld-musl-armhf.so.1
         * x86         /lib/ld-musl-i386.so.1
         * s390x       /lib/ld-musl-s390x.so.1
         * x86_64      /lib/ld-musl-x86_64.so.1
         * aarch64     /lib/ld-musl-aarch64.so.1
         * riscv64     /lib/ld-musl-riscv64.so.1
         * ppc64le     /lib/ld-musl-powerpc64le.so.1
         * loongarch64 /lib/ld-musl-loongarch64.so.1
         */

        if (strncmp(p, "/lib/ld-musl-", 13) == 0) {
            return LIBC_MUSL;
        } else {
            return LIBC_GNU;
        }
    }

    return ERROR_NOT_DETERMINED;
}

static int handle_elf64(const int fd, const char * const fp) {
    Elf64_Ehdr ehdr;

    int ret = read(fd, &ehdr, sizeof(Elf64_Ehdr));

    if (ret == -1) {
        perror(fp);
        return ERROR_READ;
    }

    if (ret != sizeof(Elf64_Ehdr)) {
        perror(fp);
        return ERROR_NOT_FULLY_READ;
    }

    ///////////////////////////////////////////////////////////

    Elf64_Phdr phdr;

    for (unsigned int i = 1; i < ehdr.e_phnum; i++) {
        ret = pread(fd, &phdr, sizeof(Elf64_Phdr), ehdr.e_phoff + i * ehdr.e_phentsize);

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != sizeof(Elf64_Phdr)) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        if (phdr.p_type != PT_INTERP) {
            continue;
        }

        char p[phdr.p_filesz];

        ret = pread(fd, p, phdr.p_filesz, phdr.p_offset);

        if (ret == -1) {
            perror(fp);
            return ERROR_READ;
        }

        if ((size_t)ret != phdr.p_filesz) {
            perror(fp);
            fprintf(stderr, "not fully read.\n");
            return ERROR_NOT_FULLY_READ;
        }

        fprintf(stderr, "%s interpreter : %s\n", fp, p);

        /*
         * default dynamic loaders for glibc:
         *
         * x86_64     /lib64/ld-linux-x86-64.so.2
         * aarch64    /lib/ld-linux-aarch64.so.1
         * riscv64    /lib/ld-linux-riscv64-lp64d.so.1
         * armhf      /lib/ld-linux-armhf.so.3
         * ppc64le    /lib/powerpc64le-linux-gnu/ld64.so.2
         * ppc64le    /lib64/ld64.so.2
         * s390x      /lib/ld64.so.1
         */

        /*
         * default dynamic loaders for musl:
         *
         * armhf       /lib/ld-musl-armhf.so.1
         * x86         /lib/ld-musl-i386.so.1
         * s390x       /lib/ld-musl-s390x.so.1
         * x86_64      /lib/ld-musl-x86_64.so.1
         * aarch64     /lib/ld-musl-aarch64.so.1
         * riscv64     /lib/ld-musl-riscv64.so.1
         * ppc64le     /lib/ld-musl-powerpc64le.so.1
         * loongarch64 /lib/ld-musl-loongarch64.so.1
         */

        if (strncmp(p, "/lib/ld-musl-", 13) == 0) {
            return LIBC_MUSL;
        } else {
            return LIBC_GNU;
        }
    }

    return ERROR_NOT_DETERMINED;
}

static int determine_by_inspect_elf_file(const char * fp) {
    struct stat st;

    if (stat(fp, &st) == -1) {
        //perror(fp);
        return ERROR_STAT;
    }

    if (st.st_size < 52) {
        //fprintf(stderr, "NOT an ELF file: %s\n", fp);
        return ERROR_NOT_ELF_FILE;
    }

    int fd = open(fp, O_RDONLY);

    if (fd == -1) {
        perror(fp);
        return ERROR_OPEN;
    }

    ///////////////////////////////////////////////////////////

    unsigned char a[5];

    ssize_t readBytes = read(fd, a, 5);

    if (readBytes == -1) {
        perror(fp);
        close(fd);
        return ERROR_READ;
    }

    if (readBytes != 5) {
        perror(fp);
        close(fd);
        fprintf(stderr, "not fully read.\n");
        return ERROR_NOT_FULLY_READ;
    }

    ///////////////////////////////////////////////////////////

    // https://www.sco.com/developers/gabi/latest/ch4.eheader.html
    if ((a[0] != 0x7F) || (a[1] != 0x45) || (a[2] != 0x4C) || (a[3] != 0x46)) {
        //fprintf(stderr, "NOT an ELF file: %s\n", fp);
        close(fd);
        return ERROR_NOT_ELF_FILE;
    }

    ///////////////////////////////////////////////////////////

    off_t offset = lseek(fd, 0, SEEK_SET);

    if (offset == -1) {
        perror(fp);
        close(fd);
        return ERROR_SEEK;
    }

    ///////////////////////////////////////////////////////////

    int ret;

    switch (a[4]) {
        case ELFCLASS32: ret = handle_elf32(fd, fp); break;
        case ELFCLASS64: ret = handle_elf64(fd, fp); break;
        default: 
            //fprintf(stderr, "Invalid ELF file: %s\n", fp);
            ret = ERROR_BROKEN_ELF_FILE;
    }

    close(fd);

    return ret;
}

int determine_by_inspect_elf_files() {
    // A Docker container usually does not have /sbin/init
    // musl-based voidlinux has a fully statically linked /sbin/init
    // https://github.com/oasislinux/oasis A GNU/Linux distribution where all executables are fully statically linked
    const char* list[2] = {"/bin/sh", "/sbin/init"};

    int ret;

    for (int i = 0; i < 2; i++) {
        ret = determine_by_inspect_elf_file(list[i]);

        if (ret == LIBC_GNU || ret == LIBC_MUSL) {
            return ret;
        }
    }

    return ret;
}

static int xx() {
    int ret = determine_by_inspect_elf_files();

    switch (ret) {
        case LIBC_GNU: puts("glibc"); return 0;
        case LIBC_MUSL: puts("musl"); return 0;
        default: return ret;
    }
}
