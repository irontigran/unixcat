#include <stdio.h>

#include "printfd.h"

void PFD_print_fd(int fd) { printf("@ANC: SCM_RIGHTS %d\n", fd); }
