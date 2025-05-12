#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "help.h"
#include "sock.h"

int main(int argc, char **argv) {
    int opt;
    bool listen = false;

    int option_index = 0;
    static struct option long_options[] = {
        (struct option) {.name="help",      .has_arg=no_argument,   .flag=NULL, .val=0},
        (struct option) {.name="version",   .has_arg=no_argument,   .flag=NULL, .val=0}
    };
    int c;
    while ((c = getopt_long(argc, argv, "+l", long_options, &option_index)) != -1) {
        if (c == -1) {
            goto usage_exit;
        }
        switch (c) {
            case 0:
                if (option_index == 0) {
                    usage(argv[0]);
                }
                if (option_index == 1) {
                    version();
                }
                exit(EXIT_SUCCESS);
                break;
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
            perror("on bind");
            exit(EXIT_FAILURE);
        }
        int clientfd;
        if ((clientfd = serv_accept(listenfd)) < 0) {
            perror("on accept");
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
