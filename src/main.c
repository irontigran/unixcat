#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cli.h"
#include "creds.h"
#include "help.h"
#include "main.h"
#include "options.h"
#include "serv.h"

static void readwrite(int net_fd, AncillaryCfg cfg);
static ssize_t Std_read(int fd, uint8_t *buf, size_t buflen);

int main(int argc, char **argv) {
    bool listen = false;
    const char *source = NULL;
    int fd;
    AncillaryCfg config;
    config.numfds = 0;
    config.send_creds = 0;
    config.recv_creds = 0;

    const char *default_shorts = "+ls:f:";
    const struct option default_longs[] = {
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
    OptBundle opts;
    opts.shortopts = NULL;
    opts.longopts = NULL;
    opts = Options_append(opts, default_shorts, default_longs);
    opts = Creds_register_options(opts);

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, opts.shortopts, opts.longopts,
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
                if (config.numfds >= SCM_MAX_FD) {
                    break;
                }
                if ((fd = open(optarg, 0)) == -1) {
                    int tmp = errno;
                    fprintf(stderr, "couldn't open %s: ", optarg);
                    errno = tmp;
                    perror(NULL);
                    break;
                }
                config.send_fds[config.numfds] = fd;
                config.numfds++;
                break;
            case 'R':
                if (strcmp("once", optarg) == 0) {
                    config.recv_creds = 1;
                } else if (strcmp("always", optarg) == 0) {
                    config.recv_creds = -1;
                } else {
                    fprintf(stderr,
                            "invalid recv creds arg: %s (valid args are 'once' "
                            "or 'always')\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }
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
        readwrite(clientfd, config);
        /*
        Serv_recv_and_print(clientfd, config);
        */
    } else {
        int clientfd;
        if ((clientfd = Cli_conn(path, source)) < 0) {
            perror("on connect");
            exit(EXIT_FAILURE);
        }
        readwrite(clientfd, config);
        /*
        Cli_send(clientfd, config);
        for (int i = 0; i < config.numfds; i++) {
            close(config.send_fds[i]);
        }
        */
        close(clientfd);
    }
    exit(EXIT_SUCCESS);
usage_exit:
    fprintf(stderr, "Usage: %s [OPTIONS] path\n", argv[0]);
    exit(EXIT_FAILURE);
}

static void readwrite(int net_fd, AncillaryCfg cfg) {
    const size_t buflen = 1024;
    uint8_t stdinbuf[buflen];
    size_t stdinpos = 0;

    // To start polling, we're interested only in reading from stdin and the
    // socket.
    const size_t plen = 3;
    struct pollfd pfds[plen];
    const size_t stdi = 0;
    const size_t neti = 1;
    const size_t neto = 2;
    pfds[stdi].fd = STDIN_FILENO;
    pfds[stdi].events = POLLIN;
    pfds[neti].fd = net_fd;
    pfds[neti].events = POLLIN;
    pfds[neto].fd = net_fd;
    pfds[neto].events = 0;

    while (1) {
        // If the socket is gone, we can't continue.
        if (pfds[neto].fd == -1) {
            return;
        }
        // If stdin is gone and the stdin buffer is empty, then we're done.
        if (pfds[stdi].fd == -1 && stdinpos == 0) {
            return;
        }

        int ready = poll(pfds, plen, -1);
        if (ready == -1) {
            perror("on poll");
            return;
        }

        for (int i = 0; i < plen; i++) {
            // If any errors, mark the file descriptor as gone.
            if (pfds[i].revents & (POLLERR | POLLNVAL)) {
                pfds[i].fd = -1;
            }
        }
        // Check for POLLHUP for reads (the connection is closed). But only
        // mark the fd as gone if there's no data to read.  (We may have gotten
        // both data and then a POLLHUP.)
        if (pfds[stdi].events & POLLIN && pfds[stdi].revents & POLLHUP &&
            !(pfds[stdi].revents & POLLIN)) {
            pfds[stdi].fd = -1;
        }
        if (pfds[neti].events & POLLIN && pfds[neti].revents & POLLHUP &&
            !(pfds[neti].revents & POLLIN)) {
            pfds[neti].fd = -1;
        }
        // Meanwhile, a POLLHUP on socket write means we just shutdown the
        // connection.
        if (pfds[neto].revents & POLLHUP) {
            if (pfds[neto].fd != -1) {
                shutdown(pfds[neto].fd, SHUT_WR);
            }
            pfds[neto].fd = -1;
        }

        ssize_t ret;
        // Read from stdin if there's data available and space in our buffer.
        if (pfds[stdi].revents & POLLIN && stdinpos < buflen) {
            ret =
                Std_read(pfds[stdi].fd, stdinbuf + stdinpos, buflen - stdinpos);
            if (ret == -1) {
                pfds[stdi].fd = -1;
            } else {
                stdinpos += ret;
            }
            // If we got some data, signal that we want to write it to the
            // socket.
            if (stdinpos > 0) {
                pfds[neto].events = POLLOUT;
            }
            // If the buffer is full, stop reading.
            if (stdinpos == buflen) {
                pfds[stdi].events = 0;
            }
        }

        // Read from the socket. Any reads from the socket are immediately
        // printed to stdout on the spot, so no need to pass buffers around.
        if (pfds[neti].revents & POLLIN) {
            if (Net_recv_and_print(pfds[neti].fd, cfg) < 0) {
                pfds[neti].fd = -1;
            }
        }

        // Write to the socket if possible and we have data in the buffer.
        if (pfds[neto].revents & POLLOUT && stdinpos > 0) {
            ret = Net_send(pfds[neto].fd, stdinbuf, stdinpos, cfg);
            if (ret == -1) {
                pfds[neto].fd = -1;
            } else {
                ssize_t adjust = stdinpos - ret;
                if (adjust > 0) {
                    memmove(stdinbuf, stdinbuf + ret, adjust);
                }
                stdinpos -= ret;
            }

            // We've made room in the buffer, we can start reading from stdin
            // again (if we ever stopped).
            if (stdinpos < buflen) {
                pfds[stdi].events = POLLIN;
            }
            // If the buffer is empty, we don't need to send anything on the
            // socket.
            if (stdinpos == 0) {
                pfds[neto].events = 0;
            }
            // We only pass file descriptors with the first message.
            cfg.numfds = 0;
        }
    }
}

static ssize_t Std_read(int fd, uint8_t *buf, size_t buflen) {
    ssize_t n = read(fd, buf, buflen);
    // Blocking and interruptions aren't errors, try again later.
    if (n <= 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
        return 0;
    }
    if (n < 0) {
        perror("on read");
    }
    return n;
}
