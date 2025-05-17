#ifndef SERV_H
#define SERV_H

#include "main.h"

int Serv_listen(const char *path);
int Serv_accept(int fd);
void Serv_recv_and_print(int fd, AncillaryCfg ac);

#endif  // SERV_H
