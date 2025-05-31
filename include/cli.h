#ifndef CLI_H
#define CLI_H

#include "main.h"

int Cli_conn(const char *dst, const char *src);
ssize_t Net_send(int fd, void *buf, size_t buflen, AncillaryCfg config);

#endif  // CLI_H
