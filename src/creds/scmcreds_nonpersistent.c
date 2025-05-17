#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "creds.h"
#include "fdstate.h"
#include "options.h"

int Creds_turn_on_once(int fd) {
    int on = 1;
    return setsockopt(fd, SOL_LOCAL, LOCAL_CREDS, &on, sizeof(int));
}

int Creds_turn_on_persistent(int fd) {
    FdState_set_persistent(fd);
    return Creds_turn_on_once(fd);
}

int Creds_confirm_recv_settings(int fd) {
    if (!FdState_is_persistent(fd)) {
        return 0;
    }
    int on = 1;
    return setsockopt(fd, SOL_LOCAL, LOCAL_CREDS, &on, sizeof(int));
}

bool Creds_is_credential(int cmsg_type) { return cmsg_type == SCM_CREDS; }

void Creds_print_credential(struct cmsghdr *cmsg) {
    struct sockcred *sockc = (struct sockcred *)CMSG_DATA(cmsg);
    printf("@ANC: SCM_CREDS uid=%d,euid=%d,gid=%d,egid=%d", sockc->sc_uid,
           sockc->sc_euid, sockc->sc_gid, sockc->sc_egid);
    if (sockc->sc_ngroups > 0) {
        printf(",supplemental groups=");
    }
    for (int i = 0; i < sockc->sc_ngroups; i++) {
        if (i != 0) {
            printf(",");
        }
        printf("%d", sockc->sc_groups[i]);
    }
    printf("\n");
}
