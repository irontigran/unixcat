#ifndef SERV_H
#define SERV_H

int Serv_listen(const char *path);
int Serv_accept(int fd);
void Serv_recv_and_print(int fd);

#endif  // SERV_H
