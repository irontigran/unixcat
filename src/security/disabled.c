#include <stdbool.h>
#include <sys/socket.h>

#include "options.h"
#include "security.h"

char *Security_help_message = "";

OptBundle Security_register_options(OptBundle existing) { return existing; }

int Security_turn_on_passsec(int fd) { return 0; }

bool Security_is_context(int cmsg_level, int cmsg_type) { return false; }

void Security_print_seccxt(struct cmsghdr *cmsg) { return; }
