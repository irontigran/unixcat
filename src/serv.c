#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "serv.h"

int Serv_listen(const char *path) {
    struct sockaddr_un un;
    if (strlen(path) >= sizeof(un.sun_path)) {
        errno = ENAMETOOLONG;
        return -1;
    }
    int fd;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return -2;
    }

    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, path);
    // Three separate options for setting the address length...
    // APUE suggests the first, the man pages suggest either of the second or
    // third.
    // int len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    // int len = offsetof(struct sockaddr_un, sun_path) + strlen(path) + 1;
    // int len = sizeof(un);
    // if (bind(fd, (struct sockaddr *)&un, len) < 0) {

    int ret;
    if (bind(fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
        ret = -3;
        goto err;
    }
    if (listen(fd, 0) < 0) {
        ret = -4;
        goto err;
    }
    return fd;
err:
    int tmperr = errno;
    close(fd);
    errno = tmperr;
    return ret;
}

int Serv_accept(int fd) {
    int newfd;
    if ((newfd = accept(fd, NULL, NULL)) < 0) {
        return -1;
    }
    return newfd;
}

void Serv_recv_and_print(int fd) {
    const size_t buflen = 256;
    char *buf[buflen];
    ssize_t recvd;
    while ((recvd = read(fd, buf, buflen)) > 0) {
        ssize_t written = 0;
        while (written < recvd) {
            ssize_t ret;
            ret = write(STDOUT_FILENO, buf + written, recvd - written);
            if (ret == 0) {
                fprintf(stderr, "unexpected EOF");
            } else if (ret < 0) {
                perror("write:");
            }
            written += ret;
        }
    }
    if (recvd < 0) {
        perror("read:");
    }
}
