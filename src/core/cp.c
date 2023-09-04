#include <errno.h>

#include <fcntl.h>
#include <unistd.h>

#include "cp.h"

int copy_file(const char * fromFilePath, const char * toFilePath) {
    int fromFD = open(fromFilePath, O_RDONLY);

    if (fromFD == -1) {
        return -1;
    }

    int toFD = open(toFilePath, O_CREAT | O_TRUNC | O_WRONLY, 0666);

    if (toFD == -1) {
        int err = errno;
        close(fromFD);
        errno = err;
        return -1;
    }

    unsigned char buf[1024];

    for (;;) {
        ssize_t readSize = read(fromFD, buf, 1024);

        if (readSize == -1) {
            int err = errno;
            close(fromFD);
            close(toFD);
            errno = err;
            return -1;
        }

        if (readSize == 0) {
            close(fromFD);
            close(toFD);
            return 0;
        }

        ssize_t writeSize = write(toFD, buf, readSize);

        if (writeSize != -1) {
            int err = errno;
            close(fromFD);
            close(toFD);
            errno = err;
            return -1;
        }

        if (writeSize != readSize) {
            close(fromFD);
            close(toFD);
            errno = EIO;
            return -1;
        }
    }
}
