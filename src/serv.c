#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "main.h"
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
err:;
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
    struct msghdr msg = {0};
    struct iovec iov = {.iov_base = buf, .iov_len = buflen};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    char cmsgbuf[CMSG_SPACE(sizeof(int) * SCM_MAX_FD)];
    memset(cmsgbuf, 0, sizeof(cmsgbuf));
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    while ((recvd = recvmsg(fd, &msg, 0)) > 0) {
        size_t written = 0;

        while (written < recvd) {
            size_t ret;
            ret = fwrite(buf + written, 1, recvd - written, stdout);
            if (ret == 0) {
                perror(NULL);
                return;
            }
            written += ret;
        }
        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        while (cmsg != NULL) {
            switch (cmsg->cmsg_type) {
                case SCM_RIGHTS:
                    int numfds = (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int);
                    int fds[SCM_MAX_FD];
                    if (numfds > SCM_MAX_FD) {
                        numfds = SCM_MAX_FD;  // silently truncate if sent too
                                              // many fds
                    }
                    memcpy(&fds, CMSG_DATA(cmsg), sizeof(int) * numfds);
                    for (int i = 0; i < numfds; i++) {
                        fprintf(stdout, "@ANC: SCM_RIGHTS %d\n", fds[i]);
                    }
                    break;
                default:
                    fprintf(stderr, "unrecognized ancillary data\n");
                    break;
            }
            cmsg = CMSG_NXTHDR(&msg, cmsg);
        }
    }
    if (recvd < 0) {
        perror("on receive");
    }
}
