#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sock.h"

int main(int argc, char **argv) {
    int opt;
    bool listen = false;
    while ((opt = getopt(argc, argv, "+l")) != -1) {
        switch (opt) {
            case 'l':
                listen = true;
                break;
            default:
                goto usage_exit;
        }
    }
    if (optind >= argc) {
        goto usage_exit;
    }
    char *path = argv[optind];
    if (listen) {
        int listenfd;
        if ((listenfd = serv_listen(path)) < 0) {
            perror("serv_listen:");
            exit(EXIT_FAILURE);
        }
        int clientfd;
        if ((clientfd = serv_accept(listenfd)) < 0) {
            perror("serv_accept:");
            exit(EXIT_FAILURE);
        }
        serv_recv_and_print(clientfd);
    } else {
        printf("Will connect to %s\n", path);
    }
    exit(EXIT_SUCCESS);
usage_exit:
    fprintf(stderr, "Usage: %s [-l] path\n", argv[0]);
    exit(EXIT_FAILURE);
}
