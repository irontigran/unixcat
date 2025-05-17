#ifndef CLI_H
#define CLI_H

#include "main.h"

int Cli_conn(const char *dst, const char *src);
void Cli_send(int fd, AncillaryCfg ac);

#endif  // CLI_H
