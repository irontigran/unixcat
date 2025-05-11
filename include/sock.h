#ifndef SOCK_H
#define SOCK_H

int serv_listen(const char *path);
int serv_accept(int fd);
void serv_recv_and_print(int fd);

#endif  // SOCK_H
