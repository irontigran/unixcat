#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "creds.h"

int Creds_turn_on_once(int fd) {
    int on = 1;
    return setsockopt(fd, SOL_LOCAL, LOCAL_CREDS, &on, sizeof(int));
}

int Creds_turn_on_persistent(int fd) {
    int on = 1;
    return setsockopt(fd, SOL_LOCAL, LOCAL_CREDS_PERSISTENT, &on, sizeof(int));
}

int Creds_confirm_recv_settings(int fd) {
    return 0;  // Receive settings are always correct because we have both
               // LOCAL_CREDS and LOCAL_CREDS_PERSISTENT to set them correctly
               // at the start.
}

bool Creds_is_credential(int cmsg_level, int cmsg_type) {
    return (cmsg_level == SOL_SOCKET &&
            (cmsg_type == SCM_CREDS || cmsg_type == SCM_CREDS2));
}

void Creds_print_credential(struct cmsghdr *cmsg) {
    switch (cmsg->cmsg_type) {
        case SCM_CREDS:;  // blank statement between label and variable
                          // declaration.
            const struct cmsgcred *cmsgc = (struct cmsgcred *)CMSG_DATA(cmsg);
            struct sockcred *sockc = (struct sockcred *)CMSG_DATA(cmsg);
            // We have to deal with struct cmsgcred _and_ struct sockcred
            // because the cmsg_type doesn't tell us which struct we've gotten.
            // To figure out which one we have, we try checking if the cmsg_len
            // matches the expected struct sockcred size.
            if (cmsg->cmsg_len == CMSG_LEN(SOCKCREDSIZE(sockc->sc_ngroups))) {
                // The size does match, print a struct sockcred.
                printf("@ANC: SCM_CREDS uid=%d,euid=%d,gid=%d,egid=%d",
                       sockc->sc_uid, sockc->sc_euid, sockc->sc_gid,
                       sockc->sc_egid);
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
            } else {
                // The size does not match, print a struct cmsgcred.
                printf("@ANC: SCM_CREDS pid=%d,uid=%d,euid=%d,gid=%d",
                       cmsgc->cmcred_pid, cmsgc->cmcred_uid, cmsgc->cmcred_euid,
                       cmsgc->cmcred_gid);
                if (cmsgc->cmcred_ngroups > 0) {
                    printf(",supplemental groups=");
                }
                for (int i = 0; i < cmsgc->cmcred_ngroups; i++) {
                    if (i != 0) {
                        printf(",");
                    }
                    printf("%d", cmsgc->cmcred_groups[i]);
                }
                printf("\n");
            }
            break;
        case SCM_CREDS2:;  // blank statement between label and variable
                           // declaration.
            const struct sockcred2 *credp = (struct sockcred2 *)CMSG_DATA(cmsg);
            printf("@ANC: SCM_CREDS2 pid=%d,uid=%d,euid=%d,gid=%d,egid=%d",
                   credp->sc_pid, credp->sc_uid, credp->sc_euid, credp->sc_gid,
                   credp->sc_egid);
            if (credp->sc_ngroups > 0) {
                printf(",supplemental groups=");
            }
            for (int i = 0; i < credp->sc_ngroups; i++) {
                if (i != 0) {
                    printf(",");
                }
                printf("%d", credp->sc_groups[i]);
            }
            printf("\n");
            break;
        default:
            // shouldn't be calling this function unless already
            // certain it's a credential message.
            break;
    }
}
