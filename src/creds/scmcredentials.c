#define _GNU_SOURCE
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "creds.h"
#include "fdstate.h"
#include "main.h"
#include "options.h"

char *Creds_help_message =
    "  -R, --recv-creds [once|always]   receive peer credentials with every "
    "message or just the first one\n"
    "  -S, --send-creds [once|always]   send peer credentials with every "
    "message or just the first one\n"
    "  --pid    <pid>   specify a pid to send\n"
    "  --uid    <uid>   specify a uid to send\n"
    "  --gid    <gid>   specify a gid to send\n";

OptBundle Creds_register_options(OptBundle existing) {
    const struct option longopts[] = {
        (struct option){.name = "recv-creds",
                        .has_arg = required_argument,
                        .flag = NULL,
                        .val = 'R'},
        (struct option){.name = "send-creds",
                        .has_arg = required_argument,
                        .flag = NULL,
                        .val = 'S'},
        (struct option){.name = "pid",
                        .has_arg = required_argument,
                        .flag = NULL,
                        .val = 0},
        (struct option){.name = "uid",
                        .has_arg = required_argument,
                        .flag = NULL,
                        .val = 0},
        (struct option){.name = "gid",
                        .has_arg = required_argument,
                        .flag = NULL,
                        .val = 0},
        (struct option){.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}};
    return Options_append(existing, "R:S:", longopts);
}

int Creds_turn_on_once(int fd) {
    int on = 1;
    return setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(int));
}

int Creds_turn_on_persistent(int fd) {
    FdState_set_persistent(fd);
    int on = 1;
    return setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(int));
}

int Creds_confirm_recv_settings(int fd) {
    if (FdState_is_persistent(fd)) {
        return 0;
    }
    int off = 0;
    return setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &off, sizeof(int));
}

bool Creds_is_credential(int cmsg_level, int cmsg_type) {
    return (cmsg_level == SOL_SOCKET && cmsg_type == SCM_CREDENTIALS);
}

void Creds_print_credential(struct cmsghdr *cmsg) {
    const struct ucred *credp = (struct ucred *)CMSG_DATA(cmsg);
    printf("@ANC: SCM_CREDENTIALS pid=%d,uid=%d,gid=%d\n", credp->pid,
           credp->uid, credp->gid);
}

size_t Creds_sizeof_send_struct(void) { return sizeof(struct ucred); }

void Creds_fill_cmsg(struct cmsghdr *cmsg, AncillaryCfg cfg) {
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_CREDENTIALS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(struct ucred));
    struct ucred ucred = {.pid = cfg.pid, .uid = cfg.uid, .gid = cfg.gid};
    memcpy(CMSG_DATA(cmsg), &ucred, sizeof(ucred));
}
