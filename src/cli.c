#define _GNU_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "cli.h"
#include "main.h"

int Cli_conn(const char *dst, const char *src) {
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

void Cli_send(int fd, AncillaryCfg config) {
    const size_t buflen = 256;
    char *buf[buflen];
    ssize_t recvd;

    bool first = true;
    while ((recvd = read(STDIN_FILENO, buf, buflen)) > 0) {
        ssize_t written = 0;

        while (written < recvd) {
            struct msghdr msg = {0};
            struct iovec iov = {.iov_base = buf + written,
                                .iov_len = recvd - written};
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;

            int numcreds = config.send_creds;
            /*
            int cmsgspace = CMSG_SPACE(sizeof(int) * config.numfds +
                                       sizeof(struct ucred) * numcreds);
                                       */
            int cmsgspace = CMSG_SPACE(sizeof(int) * config.numfds);
            char cmsgbuf[cmsgspace];
            // Set up ancillary messages if there are any and only when we send
            // the first message.
            if (first && (config.numfds > 0 || numcreds != 0)) {
                first = false;
                memset(cmsgbuf, 0, sizeof(cmsgbuf));
                msg.msg_control = cmsgbuf;
                msg.msg_controllen = sizeof(cmsgbuf);

                struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
                if (config.numfds > 0) {
                    cmsg->cmsg_level = SOL_SOCKET;
                    cmsg->cmsg_type = SCM_RIGHTS;
                    cmsg->cmsg_len = CMSG_LEN(sizeof(int) * config.numfds);
                    memcpy(CMSG_DATA(cmsg), config.send_fds,
                           sizeof(int) * config.numfds);
                    // cmsg = CMSG_NXTHDR(&msg, cmsg);
                }
                /*
                if (numcreds != 0) {
                    cmsg->cmsg_level = SOL_SOCKET;
                    cmsg->cmsg_type = SCM_CREDENTIALS;
                    cmsg->cmsg_len = CMSG_LEN(sizeof(struct ucred));
                    struct ucred *credp = (struct ucred *)CMSG_DATA(cmsg);
                    credp->uid = geteuid();
                    credp->gid = getegid();
                    credp->pid = getpid();
                }
                */
            }

            ssize_t ret;
            ret = sendmsg(fd, &msg, 0);
            if (ret == 0) {
                fprintf(stderr, "unexpected EOF\n");
                return;
            } else if (ret < 0) {
                perror("sendmsg");
                return;
            }
            written += ret;
        }
    }

    if (recvd < 0) {
        perror("read");
    }
}
