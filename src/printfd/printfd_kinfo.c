#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/user.h>

#include "printfd.h"

/* print_fd implementation when the F_KINFO command is available.
 */

void PFD_print_fd(int fd) {
    struct kinfo_file kf;
    memset(&kf, 0, sizeof(kf));
    kf.kf_structsize = sizeof(kf);
    if (fcntl(fd, F_KINFO, &kf) < 0) {
        perror("fcntl");
    }
    printf("@ANC: SCM_RIGHTS %s\n", kf.kf_path);
}
