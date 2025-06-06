#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "creds.h"
#include "options.h"

char *Creds_help_message =
    "  -R, --recv-creds [once|always]   receive peer credentials with every "
    "message or just the first one\n";

OptBundle Creds_register_options(OptBundle existing) {
    const struct option longopts[] = {
        (struct option){.name = "recv-creds",
                        .has_arg = required_argument,
                        .flag = NULL,
                        .val = 'R'},
        (struct option){.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}};
    return Options_append(existing, "R:", longopts);
}

size_t Creds_sizeof_send_struct(void) { return 0; }

void Creds_fill_cmsg(struct cmsghdr *cmsg, AncillaryCfg cfg) {
    (void)cfg;
    (void)cmsg;
}
