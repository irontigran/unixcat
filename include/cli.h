#ifndef CLI_H
#define CLI_H

#include "main.h"

int Cli_conn(const char *dst, const char *src);
void Cli_send(int fd, const int send_fds[SCM_MAX_FD], int numfds);

#endif  // CLI_H
