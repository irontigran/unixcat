#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int Cli_conn(const char *dst, const char *src) {
    struct sockaddr_un to;
    if (strlen(dst) >= sizeof(to.sun_path)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    int fd, ret;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    // If provided a source address, we'll try to bind our socket to that
    // address before connecting.
    if (src != NULL) {
        struct sockaddr_un from;
        if (strlen(src) >= sizeof(from.sun_path)) {
            errno = ENAMETOOLONG;
            ret = -1;
            goto err;
        }
        memset(&from, 0, sizeof(from));
        from.sun_family = AF_UNIX;
        strcpy(from.sun_path, src);
        if (bind(fd, (struct sockaddr *)&from, sizeof(from)) < 0) {
            ret = -2;
            goto err;
        }
    }

    memset(&to, 0, sizeof(to));
    to.sun_family = AF_UNIX;
    strcpy(to.sun_path, dst);

    if (connect(fd, (struct sockaddr *)&to, sizeof(to)) < 0) {
        ret = -3;
        goto err;
    }
    return fd;
err:
    int tmp = errno;
    close(fd);
    errno = tmp;
    return ret;
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
