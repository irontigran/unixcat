#ifndef CREDS_H
#define CREDS_H

#include <stdbool.h>
#include <sys/socket.h>

#include "options.h"

extern char *Creds_help_message;

OptBundle Creds_register_options(OptBundle existing);
int Creds_turn_on_once(int fd);
int Creds_turn_on_persistent(int fd);
int Creds_confirm_recv_settings(int fd);
bool Creds_is_credential(int cmsg_type);
void Creds_print_credential(struct cmsghdr *cmsg);

#endif  // CREDS_H
