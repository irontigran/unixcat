#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "creds.h"
#include "main.h"
#include "options.h"

char *Creds_help_message =
    "  -R, --recv-creds [once|always]   receive peer credentials with every "
    "message or just the first one\n"
    "  -S, --send-creds [once|always]   send peer credentials with every "
    "message or just the first one\n";

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
        (struct option){.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}};
    return Options_append(existing, "R:S:", longopts);
}

size_t Creds_sizeof_send_struct(void) { return sizeof(struct cmsgcred); }

void Creds_fill_cmsg(struct cmsghdr *cmsg, AncillaryCfg cfg) {
    (void)cfg;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_CREDS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(struct cmsgcred));
    memset(CMSG_DATA(cmsg), 0, sizeof(struct cmsgcred));
}
