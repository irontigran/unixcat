#include <stdio.h>
#include <sys/stat.h>

#include "printfd.h"

void PFD_print_fd(int fd) {
    struct stat sb;
    if (fstat(fd, &sb) < 0) {
        perror("stat");
    }
    printf("@ANC: SCM_RIGHTS ");
    switch (st.st_mode & S_IFMT) {
        case S_IFSOCK:
            printf("[socket]");
            break;
        case S_IFLNK:
            printf("[link]");
            break;
        case S_IFREG:
            printf("[file]");
            break;
        case S_IFBLK:
            printf("[block dev]");
            break;
        case S_IFDIR:
            printf("[directory]");
            break;
        case S_IFCHR:
            printf("[char dev]");
            break;
        case S_IFIFO:
            printf("[pipe]");
            break;
        default:
            printf("[?]");
            break;
    }
    printf("\n");
}
