#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include <elf.h>

#define ERROR_STAT               100
#define ERROR_OPEN               101
#define ERROR_MMAP               102

#define ERROR_NOT_ELF_FILE       120
#define ERROR_BROKEN_ELF_FILE    130

#define LIBC_UNKNOWN 0
#define LIBC_GNU     1
#define LIBC_MUSL    2

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

static inline __attribute__((always_inline)) int handle_elf64(const unsigned char * elf) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(elf + ehdr->e_phoff);

    for (Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_INTERP) {
            const char * p = (const char *)(elf + phdr[i].p_offset);

            //fprintf(stderr, "interpreter : %s\n", p);

            if (strncmp(p, "/lib/ld-musl-", 13) == 0) {
                return LIBC_MUSL;
            } else {
                return LIBC_GNU;
            }
        }
    }

    return LIBC_UNKNOWN;
}

static inline __attribute__((always_inline)) int handle_elf32(const unsigned char * elf) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf;
    Elf32_Phdr *phdr = (Elf32_Phdr *)(elf + ehdr->e_phoff);

    for (Elf32_Half i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_INTERP) {
            const char * p = (const char *)(elf + phdr[i].p_offset);

            if (strncmp(p, "/lib/ld-musl-", 13) == 0) {
                return LIBC_MUSL;
            } else {
                return LIBC_GNU;
            }
        }
    }

    return LIBC_UNKNOWN;
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

    void * data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return ERROR_MMAP;
    }

    close(fd);

    unsigned char * elf = (unsigned char *)data;

    if (memcmp(elf, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "Not an ELF file: %s\n", fp);
        munmap(data, st.st_size);
        close(fd);
        return ERROR_NOT_ELF_FILE;
    }

    int ret;

    switch (elf[EI_CLASS]) {
        case ELFCLASS64: ret = handle_elf64(elf); break;
        case ELFCLASS32: ret = handle_elf32(elf); break;
        default:
            fprintf(stderr, "invalid ELF file: %s\n", fp);
            ret = ERROR_BROKEN_ELF_FILE;
    }

    munmap(data, st.st_size);
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
