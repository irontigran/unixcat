#define _GNU_SOURCE
#include <errno.h>
#include <limits.h>
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
#include "security.h"

int fill_unix_sockaddr(struct sockaddr_un *addr, const char *path);

int Net_conn(const char *dst, int proto, char *src) {
    struct sockaddr_un to;

    int fd, ret, tmperr;
    if ((fd = socket(AF_UNIX, proto, 0)) < 0) {
        return -1;
    }
    // If provided a source address, we'll try to bind our socket to that
    // address before connecting.  If using a datagram socket, we _must_ have a
    // source address.
    struct sockaddr_un from;
    int addrlen;
    if (strlen(src) > 0 || proto == SOCK_DGRAM) {
        if (strlen(src) > 0) {
            addrlen = fill_unix_sockaddr(&from, src);
            if (addrlen < 0) {
                ret = -2;
                goto err;
            }
        } else {
#define TEMPLATE "/tmp/ucat.XXXXXX"
            strcpy(src, TEMPLATE);
            if (mkstemp(src) < 0) {
                perror("mkstemp");
                ret = -3;
                goto err;
            }
            addrlen = fill_unix_sockaddr(&from, src);
            if (addrlen < 0) {
                ret = -4;
                goto err;
            }
            unlink(from.sun_path);
        }
        if (bind(fd, (struct sockaddr *)&from, addrlen) < 0) {
            ret = -5;
            goto err;
        }
    }

    addrlen = fill_unix_sockaddr(&to, dst);
    if (addrlen < 0) {
        ret = -6;
        goto err;
    }

    if (connect(fd, (struct sockaddr *)&to, addrlen) < 0) {
        ret = -4;
        goto err;
    }
    return fd;
err:
    unlink(from.sun_path);
    tmperr = errno;
    close(fd);
    errno = tmperr;
    return ret;
}

int Net_bind(const char *path, int proto) {
    int fd;
    if ((fd = socket(AF_UNIX, proto, 0)) < 0) {
        return -1;
    }

    struct sockaddr_un un;
    int ret;
    int addrlen = fill_unix_sockaddr(&un, path);
    if (addrlen < 0) {
        ret = -2;
        goto err;
    }

    if (bind(fd, (struct sockaddr *)&un, addrlen) < 0) {
        ret = -3;
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
    if (listen(fd, 0) < 0) {
        return -1;
    }
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

    int cmsgspace = 0;
    if (config.numfds > 0) {
        cmsgspace += CMSG_SPACE(sizeof(int) * config.numfds);
    }
    if (config.send_creds != 0) {
        cmsgspace += CMSG_SPACE(Creds_sizeof_send_struct());
    }
    char cmsgbuf[cmsgspace];
    memset(cmsgbuf, 0, sizeof(cmsgbuf));

    if (config.numfds > 0 || config.send_creds != 0) {
        msg.msg_control = cmsgbuf;
        msg.msg_controllen = sizeof(cmsgbuf);

        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        if (config.numfds > 0) {
            cmsg->cmsg_level = SOL_SOCKET;
            cmsg->cmsg_type = SCM_RIGHTS;
            cmsg->cmsg_len = CMSG_LEN(sizeof(int) * config.numfds);
            memcpy(CMSG_DATA(cmsg), config.send_fds,
                   sizeof(int) * config.numfds);
            cmsg = CMSG_NXTHDR(&msg, cmsg);
        }
        if (config.send_creds != 0) {
            Creds_fill_cmsg(cmsg, config);
        }
    }

    ssize_t ret = sendmsg(fd, &msg, 0);
    if (ret < 0) {
        perror("sendmsg");
    }
    return ret;
}

int Net_recv_and_print(int fd) {
    const size_t buflen = 1024;
    uint8_t buf[buflen];
    struct msghdr msg = {0};
    struct iovec iov = {.iov_base = buf, .iov_len = buflen};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // Even when providing this much space, it's still theoretically possible
    // to provide a longer cmsg then we have space for. You'd have to work
    // pretty hard to do it, and it is only possible on some BSDs where there
    // are lots and lots of supplemental groups, so we'll still need to check
    // for truncation later.
    const size_t max_cmsg = CMSG_SPACE(sizeof(int) * SCM_MAX_FD) +
                            CMSG_SPACE(Creds_sizeof_send_struct()) +
                            CMSG_SPACE(NAME_MAX);

    char cmsgbuf[max_cmsg];
    memset(cmsgbuf, 0, sizeof(cmsgbuf));
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    ssize_t recvd;
    while ((recvd = recvmsg(fd, &msg, MSG_DONTWAIT)) > 0) {
        // On some systems we're mimicking sending credentials just once using
        // a persistent mechanism. Or vice versa: we're mimicking sending
        // credentials persistently using a just once mechanism. So even though
        // we set the credential options on the socket before starting any
        // networking operations, we need to confirm the options after every
        // recvmsg call.  We might need to turn an option off after one revmsg
        // call; or turn it on again after every recvmsg call. On some systems
        // this is just a no-op.
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

        if (msg.msg_flags & MSG_CTRUNC) {
            printf("@ANC warning: control messages truncated\n");
        }

        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        int numfds;
        int fds[SCM_MAX_FD];
        while (cmsg != NULL) {
            if (cmsg->cmsg_level == SOL_SOCKET &&
                cmsg->cmsg_type == SCM_RIGHTS) {
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
            } else if (Creds_is_credential(cmsg->cmsg_level, cmsg->cmsg_type)) {
                Creds_print_credential(cmsg);
            } else if (Security_is_context(cmsg->cmsg_level, cmsg->cmsg_type)) {
                Security_print_seccxt(cmsg);
            } else {
                fprintf(stderr, "unrecognized ancillary data\n");
            }
            cmsg = CMSG_NXTHDR(&msg, cmsg);
        }
    }
    if (recvd < 0 &&
        (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
        return -2;
    }
    if (recvd < 0) {
        perror("on receive");
    }
    return recvd;
}
