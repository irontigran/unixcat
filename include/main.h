#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <sys/types.h>

// Linux-specific limit, but 253 file descriptors should be enough for most.
#define SCM_MAX_FD 253

/* Controls the behavior around sending and receiving ancillary messages.
 */
typedef struct ancillary_cfg {
    // The list of file descriptors to send. Set numfds to 0 if there aren't
    // any.
    int numfds;
    int send_fds[SCM_MAX_FD];

    // For both send_ and recv_creds, a negative value means send or receive
    // creds on every message, 0 means don't send/recv creds, and a positive
    // number means send/recv creds only on the first message. Possible
    // extension: positive number means send/recv creds on the first n
    // messages.
    int send_creds;
    int recv_creds;

    // Linux only: set the pid/uid/gid of credentials. Ignored by other
    // systems.
    pid_t pid;
    uid_t uid;
    gid_t gid;

    // Linux only: turn on passing an SELinux context.
    bool security;
} AncillaryCfg;

#endif  // MAIN_H
