#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <sys/types.h>

// Linux-specific limit, but 253 file descriptors should be enough for most.
#define SCM_MAX_FD 253

typedef struct ancillary_cfg {
    int numfds;
    int send_fds[SCM_MAX_FD];
    int send_creds;  // 0 means don't, negative means forever.
    int recv_creds;
    pid_t pid;
    uid_t uid;
    gid_t gid;
} AncillaryCfg;

#endif  // MAIN_H
