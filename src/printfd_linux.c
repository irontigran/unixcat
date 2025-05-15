#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "printfd.h"

void PFD_print_fd(int fd) {
    char *procfs;
    int ret = asprintf(&procfs, "/proc/self/fd/%d", fd);
    if (ret < 0) {
        return;
    }

    struct stat sb;
    if (lstat(procfs, &sb) < 0) {
        perror("stat");
    }

    int bufsize = 256;
    char *pathbuf = NULL;
    while (1) {
        pathbuf = malloc(bufsize);
        if (!pathbuf) {
            free(procfs);
            return;
        }
        memset(pathbuf, 0, bufsize);
        int n = readlink(procfs, pathbuf, bufsize - 1);
        if (n == bufsize - 1) {
            // If readlink reports that the path was truncated, reallocate and
            // try again.
            bufsize *= 2;
            free(pathbuf);
            continue;
        }
        break;
    }
    printf("@ANC: SCM_RIGHTS %s\n", pathbuf);
    free(procfs);
    free(pathbuf);
}
