#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cli.h"
#include "help.h"
#include "main.h"
#include "serv.h"

int main(int argc, char **argv) {
    bool listen = false;
    const char *source = NULL;
    int fd;
    int numfds = 0;
    int fds[SCM_MAX_FD];

    int option_index = 0;
    static struct option long_options[] = {
        (struct option){
            .name = "help", .has_arg = no_argument, .flag = NULL, .val = 0},
        (struct option){
            .name = "version", .has_arg = no_argument, .flag = NULL, .val = 0},
        (struct option){
            .name = "listen", .has_arg = no_argument, .flag = NULL, .val = 'l'},
        (struct option){.name = "source",
                        .has_arg = required_argument,
                        .flag = NULL,
                        .val = 's'},
        (struct option){.name = "fd",
                        .has_arg = required_argument,
                        .flag = NULL,
                        .val = 'f'},
        (struct option){.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}};
    int c;
    while ((c = getopt_long(argc, argv, "+ls:f:", long_options,
                            &option_index)) != -1) {
        switch (c) {
            case 0:
                if (option_index == 0) {
                    usage(argv[0]);
                    exit(EXIT_SUCCESS);
                }
                if (option_index == 1) {
                    version();
                    exit(EXIT_SUCCESS);
                }
                break;
            case 'l':
                listen = true;
                break;
            case 's':
                source = optarg;
                break;
            case 'f':
                if (numfds >= SCM_MAX_FD) {
                    fprintf(stderr, "too many files\n");
                    break;
                }
                if ((fd = open(optarg, 0)) == -1) {
                    int tmp = errno;
                    fprintf(stderr, "couldn't open %s: ", optarg);
                    errno = tmp;
                    perror(NULL);
                    break;
                }
                fds[numfds] = fd;
                numfds++;
                break;
            default:
                goto usage_exit;
        }
    }

    if (optind >= argc) {
        goto usage_exit;
    }
    const char *path = argv[optind];
    if (listen) {
        int listenfd;
        if ((listenfd = Serv_listen(path)) < 0) {
            perror("on bind");
            exit(EXIT_FAILURE);
        }
        int clientfd;
        if ((clientfd = Serv_accept(listenfd)) < 0) {
            perror("on accept");
            exit(EXIT_FAILURE);
        }
        Serv_recv_and_print(clientfd);
    } else {
        int clientfd;
        if ((clientfd = Cli_conn(path, source)) < 0) {
            perror("on connect");
            exit(EXIT_FAILURE);
        }
        Cli_send(clientfd, fds, numfds);
    }
    exit(EXIT_SUCCESS);
usage_exit:
    fprintf(stderr, "Usage: %s [-l] path\n", argv[0]);
    exit(EXIT_FAILURE);
}
