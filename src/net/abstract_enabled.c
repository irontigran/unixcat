#include <errno.h>
#include <stddef.h>
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
    if (path[0] == '@') {
        addr->sun_path[0] = '\0';
        strcpy(addr->sun_path+1, path + 1);
        return offsetof(struct sockaddr_un, sun_path) + 1 + strlen(addr->sun_path + 1);
    } else {
        strcpy(addr->sun_path, path);
        return sizeof(struct sockaddr_un);
    }
}
