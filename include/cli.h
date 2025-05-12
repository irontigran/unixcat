#ifndef CLI_H
#define CLI_H

int Cli_conn(const char *dst, const char *src);
void Cli_send(int fd);

#endif  // CLI_H
