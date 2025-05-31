#ifndef SERV_H
#define SERV_H

#include "main.h"

int Serv_listen(const char *path);
int Serv_accept(int fd);
int Net_recv_and_print(int fd, AncillaryCfg config);

#endif  // SERV_H
