#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>

#include "sock.h"

int serv_listen(const char *path) {
    struct sockaddr_un un;
    int fd, ret, err;
    if (strlen(path) >= sizeof(un.sun_path)) {
        errno = ENAMETOOLONG;
        return -1;
    }
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return -2;
    }
    unlink(path);

    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, path);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(path);

    if (bind(fd, (struct sockaddr *)&un, len) < 0) {
        ret = -3;
        goto err;
    }
    if (listen(fd, 0) < 0) {
        ret = -4;
        goto err;
    }
    return fd;
err:
    err = errno;
    close(fd);
    errno = err;
    return ret;
}

