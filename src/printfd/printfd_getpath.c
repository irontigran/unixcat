#include <fcntl.h>
#include <stdio.h>
#include <sys/param.h>

#include "printfd.h"

/* print_fd implementation when the F_GETPATH command is available.
 */

void PFD_print_fd(int fd) {
    char pathbuf[MAXPATHLEN] = {0};
    if (fcntl(fd, F_GETPATH, &pathbuf) < 0) {
        perror("fcntl");
    }
    printf("@ANC: SCM_RIGHTS %s\n", pathbuf);
}
