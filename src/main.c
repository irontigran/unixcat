#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        printf("Will listen on %s\n", path);
    } else {
        printf("Will connect to %s\n", path);
    }
    exit(EXIT_SUCCESS);
usage_exit:
    fprintf(stderr, "Usage: %s [-l] path\n", argv[0]);
    exit(EXIT_FAILURE);
}
