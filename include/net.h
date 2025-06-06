#ifndef NET_H
#define NET_H

#include "main.h"

/* Connect to a unix domain socket.
 *
 * dst is the pathname of a unix domain socket. src is the unix domain socket
 * to bind to before connection. If src is NULL, then it won't bind to a source
 * address.
 *
 * Returns the new connected socket file descriptor, or a negative integer on
 * error.
 */
int Net_conn(const char *dst, const char *src);

/* Listen on a unix domain socket.
 *
 * path is the pathname to listen on. Will return an error if the path already
 * exists.
 *
 * Returns the new listening socket file descriptor, or a negative integer on
 * error.
 */
int Net_listen(const char *path);

/* Accept an incoming connection on a listening socket.
 *
 * fd is the listening socket file descriptor.
 *
 * Returns the new connected socket file descriptor, or a negative integer on
 * error.
 */
int Net_accept(int fd);

/* Send data and ancillary messages over a unix socket descriptor. (Does not
 * block.)
 *
 * fd is the file descriptor, buf is the data, and buflen is the amount of data
 * to send. The config parameter describes how to send ancillary data (see the
 * struct definition for more).
 *
 * Returns the number of bytes sent or a negative number on error. 0 is not
 * necessarily an error.
 */
ssize_t Net_send(int fd, void *buf, size_t buflen, AncillaryCfg config);

/* Receive data and ancillary messages over a unix socket descriptor. (Does not
 * block.) It immediately prints any data it receives to stdout.
 *
 * fd is the file descriptor
 *
 * Returns 0 on success and a negative number on error.
 */
int Net_recv_and_print(int fd);

#endif  // CLI_H
