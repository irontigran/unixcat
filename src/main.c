#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli.h"
#include "help.h"
#include "serv.h"

int main(int argc, char **argv) {
    bool listen = false;

    int option_index = 0;
    static struct option long_options[] = {
        (struct option){
            .name = "help", .has_arg = no_argument, .flag = NULL, .val = 0},
        (struct option){
            .name = "version", .has_arg = no_argument, .flag = NULL, .val = 0},
        (struct option){
            .name = "listen", .has_arg = no_argument, .flag = NULL, .val = 'l'}};
    int c;
    while ((c = getopt_long(argc, argv, "+l", long_options, &option_index)) !=
           -1) {
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
        if ((clientfd = Cli_conn(path)) < 0) {
            perror("on connect");
            exit(EXIT_FAILURE);
        }
        Cli_send(clientfd);
    }
    exit(EXIT_SUCCESS);
usage_exit:
    fprintf(stderr, "Usage: %s [-l] path\n", argv[0]);
    exit(EXIT_FAILURE);
}
