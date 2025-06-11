#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>

#include "options.h"
#include "security.h"

char *Security_help_message =
    "  --security   send the SELinux security context\n";

OptBundle Security_register_options(OptBundle existing) {
    const struct option longopts[] = {
        (struct option){
            .name = "security", .has_arg = no_argument, .flag = NULL, .val = 0},
        (struct option){.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}};
    return Options_append(existing, "", longopts);
}

int Security_turn_on_passsec(int fd) {
    int on = 1;
    return setsockopt(fd, SOL_SOCKET, SO_PASSSEC, &on, sizeof(on));
}

bool Security_is_context(int cmsg_level, int cmsg_type) {
    // SCM_SECURITY is defined in the kernel, but some/most libcs don't define
    // it.
#ifndef SCM_SECURITY
#define SCM_SECURITY 0x3
#endif
    return (cmsg_level == SOL_SOCKET && cmsg_type == SCM_SECURITY);
}

void Security_print_seccxt(struct cmsghdr *cmsg) {
    printf("@ANC: SCM_SECURITY %s\n", CMSG_DATA(cmsg));
}
