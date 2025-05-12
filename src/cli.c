#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int Cli_conn(const char *path) {
    struct sockaddr_un un;
    if (strlen(path) >= sizeof(un.sun_path)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    int fd;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, path);

    if (connect(fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
        return -2;
    }
    return fd;
}

void Cli_send(int fd) {
    const size_t buflen = 256;
    char *buf[buflen];
    ssize_t recvd;
    while ((recvd = read(STDIN_FILENO, buf, buflen)) > 0) {
        ssize_t written = 0;
        while (written < recvd) {
            ssize_t ret;
            ret = write(fd, buf + written, recvd - written);
            if (ret == 0) {
                fprintf(stderr, "unexpected EOF\n");
            } else if (ret < 0) {
                perror("write");
            }
            written += ret;
        }
    }
    if (recvd < 0) {
        perror("read");
    }
}
