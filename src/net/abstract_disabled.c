#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "net.h"

int fill_unix_sockaddr(struct sockaddr_un *addr, const char *path) {
    if (strlen(path) >= sizeof(addr->sun_path)) {
        errno = ENAMETOOLONG;
        return -1;
    }
    memset(addr, 0, sizeof(struct sockaddr_un));
    addr->sun_family = AF_UNIX;
    strcpy(addr->sun_path, path);
    return sizeof(struct sockaddr_un);
}
