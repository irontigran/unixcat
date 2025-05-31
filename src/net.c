#define _GNU_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "creds.h"
#include "main.h"
#include "net.h"
#include "printfd.h"

int Net_conn(const char *dst, const char *src) {
    struct sockaddr_un to;
    if (strlen(dst) >= sizeof(to.sun_path)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    int fd, ret, tmperr;
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
    tmperr = errno;
    close(fd);
    errno = tmperr;
    return ret;
}

int Net_listen(const char *path) {
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
    // if (bind(fd, (struct sockaddr *)&un, len) < 0)

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

int Net_accept(int fd) {
    int newfd;
    if ((newfd = accept(fd, NULL, NULL)) < 0) {
        return -1;
    }
    return newfd;
}

ssize_t Net_send(int fd, void *buf, size_t buflen, AncillaryCfg config) {
    struct msghdr msg = {0};
    struct iovec iov = {.iov_base = buf, .iov_len = buflen};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    int cmsgspace = CMSG_SPACE(sizeof(int) * config.numfds);
    char cmsgbuf[cmsgspace];
    if (config.numfds > 0) {
        memset(cmsgbuf, 0, sizeof(cmsgbuf));
        msg.msg_control = cmsgbuf;
        msg.msg_controllen = sizeof(cmsgbuf);

        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int) * config.numfds);
        memcpy(CMSG_DATA(cmsg), config.send_fds, sizeof(int) * config.numfds);
    }

    ssize_t ret = sendmsg(fd, &msg, 0);
    if (ret < 0) {
        perror("sendmsg");
    }
    return ret;
}

int Net_recv_and_print(int fd, AncillaryCfg config) {
    const size_t buflen = 1024;
    uint8_t buf[buflen];
    struct msghdr msg = {0};
    struct iovec iov = {.iov_base = buf, .iov_len = buflen};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    char cmsgbuf[CMSG_SPACE(sizeof(int) * SCM_MAX_FD)];
    memset(cmsgbuf, 0, sizeof(cmsgbuf));
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    // Enabling receiving passed credentials portably is tricky. Some systems
    // default to one time only, some default to receiving credentials with
    // every message. To make everything work, we have to have separate
    // functions for enabling passed credentials just once or persistently.
    if (config.recv_creds > 0) {
        if (Creds_turn_on_once(fd) < 0) {
            perror("enabling creds once");
            return -1;
        }
    }
    if (config.recv_creds < 0) {
        if (Creds_turn_on_persistent(fd) < 0) {
            perror("enabling creds always");
            return -1;
        }
    }

    ssize_t recvd;
    while ((recvd = recvmsg(fd, &msg, MSG_DONTWAIT)) > 0) {
        // In addition, on some systems we're mimicking sending credentials
        // just once using a persistent mechanism. Or vice versa: we're
        // mimicking sending credentials persistently using a just once
        // mechanism. So we need to turn it off after sending it once; or turn
        // it on after sending it once.
        if (Creds_confirm_recv_settings(fd) < 0) {
            perror("reenabling creds");
            return -1;
        }
        size_t written = 0;
        while (written < recvd) {
            size_t ret;
            ret = fwrite(buf + written, 1, recvd - written, stdout);
            if (ret == 0) {
                perror(NULL);
                return -1;
            }
            written += ret;
        }
        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        int numfds;
        int fds[SCM_MAX_FD];
        while (cmsg != NULL) {
            if (cmsg->cmsg_type == SCM_RIGHTS) {
                numfds = (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int);
                if (numfds > SCM_MAX_FD) {
                    numfds = SCM_MAX_FD;  // silently truncate if sent too
                                          // many fds
                }
                memcpy(&fds, CMSG_DATA(cmsg), sizeof(int) * numfds);
                for (int i = 0; i < numfds; i++) {
                    PFD_print_fd(fds[i]);
                    close(fds[i]);
                }
            } else if (Creds_is_credential(cmsg->cmsg_type)) {
                Creds_print_credential(cmsg);
            } else {
                fprintf(stderr, "unrecognized ancillary data\n");
            }
            cmsg = CMSG_NXTHDR(&msg, cmsg);
        }
    }
    if (recvd < 0 &&
        (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
        return 0;
    }
    if (recvd < 0) {
        perror("on receive");
    }
    return recvd;
}
