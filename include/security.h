#ifndef SECURITY_H
#define SECURITY_H

#include <sys/socket.h>

#include "options.h"

extern char *Security_help_message;

OptBundle Security_register_options(OptBundle existing);
int Security_turn_on_passsec(int fd);
bool Security_is_context(int cmsg_level, int cmsg_type);
void Security_print_seccxt(struct cmsghdr *cmsg);

#endif  // SECURITY_H
