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
#include <sys/un.h>
#include <unistd.h>

#include "creds.h"
#include "help.h"
#include "main.h"
#include "net.h"
#include "options.h"
#include "security.h"

static void readwrite(int net_fd, AncillaryCfg cfg);
static ssize_t Std_read(int fd, uint8_t *buf, size_t buflen);

int main(int argc, char **argv) {
    bool listen = false;
    int proto = SOCK_STREAM;
    // Make sure the source address is large enough to hold a valid socket
    // address path.
    char source[sizeof(((struct sockaddr_un *)0)->sun_path) + 1] = {0};
    int fd;
    AncillaryCfg config;
    config.numfds = 0;
    config.send_creds = 0;
    config.recv_creds = 0;
    config.security = false;
    config.pid = getpid();
    config.uid = getuid();
    config.gid = getgid();

    const char *default_shorts = "+ls:uf:";
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
        (struct option){
            .name = "udp", .has_arg = no_argument, .flag = NULL, .val = 'u'},
        (struct option){
            .name = "seq", .has_arg = no_argument, .flag = NULL, .val = 0},
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
    opts = Security_register_options(opts);

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, opts.shortopts, opts.longopts,
                            &option_index)) != -1) {
        switch (c) {
            case 0:
                if (strcmp(opts.longopts[option_index].name, "help") == 0) {
                    usage(argv[0]);
                    exit(EXIT_SUCCESS);
                }
                if (strcmp(opts.longopts[option_index].name, "version") == 0) {
                    version();
                    exit(EXIT_SUCCESS);
                }
                if (strcmp(opts.longopts[option_index].name, "seq") == 0) {
                    if (proto == SOCK_DGRAM) {
                        fprintf(stderr,
                                "only one of -u / --udp and --seq allowed\n");
                        exit(EXIT_FAILURE);
                    }
                    proto = SOCK_SEQPACKET;
                }
                if (strcmp(opts.longopts[option_index].name, "pid") == 0) {
                    if (!atopid(optarg, &config.pid)) {
                        exit(EXIT_FAILURE);
                    }
                    if (config.send_creds == 0) {
                        config.send_creds = 1;
                    }
                }
                if (strcmp(opts.longopts[option_index].name, "uid") == 0) {
                    if (!atouid(optarg, &config.uid)) {
                        exit(EXIT_FAILURE);
                    }
                    if (config.send_creds == 0) {
                        config.send_creds = 1;
                    }
                }
                if (strcmp(opts.longopts[option_index].name, "gid") == 0) {
                    if (!atogid(optarg, &config.gid)) {
                        exit(EXIT_FAILURE);
                    }
                    if (config.send_creds == 0) {
                        config.send_creds = 1;
                    }
                }
                if (strcmp(opts.longopts[option_index].name, "security") == 0) {
                    config.security = true;
                }
                break;
            case 'l':
                listen = true;
                break;
            case 's':
                if (strlen(optarg) < sizeof(source)) {
                    strcpy(source, optarg);
                } else {
                    fprintf(stderr,
                            "source address path too long; must be shorter "
                            "than %zu\n",
                            sizeof(source));
                    exit(EXIT_FAILURE);
                }
                break;
            case 'u':
                if (proto == SOCK_SEQPACKET) {
                    fprintf(stderr,
                            "only one of -u / --udp and --seq allowed\n");
                    exit(EXIT_FAILURE);
                }
                proto = SOCK_DGRAM;
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
            case 'S':
                if (strcmp("once", optarg) == 0) {
                    config.send_creds = 1;
                } else if (strcmp("always", optarg) == 0) {
                    config.send_creds = -1;
                } else {
                    fprintf(stderr,
                            "invalid send creds arg: %s (valid args are 'once' "
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
    int clientfd;
    if (listen) {
        int listenfd;
        if ((listenfd = Net_bind(path, proto)) < 0) {
            perror("on bind");
            exit(EXIT_FAILURE);
        }
        if (config.recv_creds > 0) {
            if (Creds_turn_on_once(listenfd) < 0) {
                perror("enabling creds once");
                exit(EXIT_FAILURE);
            }
        }
        if (config.recv_creds < 0) {
            if (Creds_turn_on_persistent(listenfd) < 0) {
                perror("enabling creds always");
                exit(EXIT_FAILURE);
            }
        }
        if (proto != SOCK_DGRAM) {
            // For connection-oriented sockets, we have to listen and accept a
            // connection to get a socket to send data on.
            if ((clientfd = Net_accept(listenfd)) < 0) {
                perror("on accept");
                exit(EXIT_FAILURE);
            }
        } else {
            // For datagram sockets, the socket we created _is_ the socket to
            // send data on. Once we get some data, we connect back to it so
            // that we only send a receive data from the socket we got data
            // from initially.
            clientfd = listenfd;
            char buf[256];
            struct sockaddr_storage peer;
            socklen_t peerlen = sizeof(peer);
            if (recvfrom(clientfd, buf, sizeof(buf), MSG_PEEK,
                         (struct sockaddr *)&peer, &peerlen) < 0) {
                perror("recvfrom");
                exit(EXIT_FAILURE);
            }
            if (connect(clientfd, (struct sockaddr *)&peer, peerlen) < 0) {
                perror("connect");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        if ((clientfd = Net_conn(path, proto, source)) < 0) {
            perror("on connect");
            exit(EXIT_FAILURE);
        }
    }

    // Enabling receiving passed credentials portably is tricky. Some systems
    // default to one time only, some default to receiving credentials with
    // every message. To make everything work, we have to have separate
    // functions for enabling passed credentials just once or persistently.
    // Some systems also want the option to be enabled on the listening socket
    // in order to work.  To cover all of our bases, we enable any relevant
    // options on both the listening socket and the client socket.
    if (config.recv_creds > 0) {
        if (Creds_turn_on_once(clientfd) < 0) {
            perror("enabling creds once");
            exit(EXIT_FAILURE);
        }
    }
    if (config.recv_creds < 0) {
        if (Creds_turn_on_persistent(clientfd) < 0) {
            perror("enabling creds always");
            exit(EXIT_FAILURE);
        }
    }
    if (config.security) {
        if (Security_turn_on_passsec(clientfd) < 0) {
            perror("enabling passsec");
            exit(EXIT_FAILURE);
        }
    }

    readwrite(clientfd, config);
    if (strlen(source) > 0 && proto == SOCK_DGRAM) {
        unlink(source);
    }
    exit(EXIT_SUCCESS);
usage_exit:
    fprintf(stderr, "Usage: %s [OPTIONS] path\n", argv[0]);
    exit(EXIT_FAILURE);
}

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
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

    if (set_nonblocking(pfds[stdi].fd) != 0 ||
        set_nonblocking(pfds[neti].fd) != 0) {
        perror("nonblocking");
        return;
    }

    while (1) {
        // If the socket is gone, we can't continue.
        if (pfds[neto].fd == -1) {
            return;
        }
        // If all the inputs are gone and input buf is empty, we're done.
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
        // If the socket out is gone, no need to keep watching stdin.
        if (pfds[neto].fd == -1) {
            pfds[stdi].fd = -1;
        }

        ssize_t ret;
        // Read from stdin if there's data available and space in our buffer.
        if (pfds[stdi].revents & POLLIN && stdinpos < buflen) {
            ret =
                Std_read(pfds[stdi].fd, stdinbuf + stdinpos, buflen - stdinpos);
            if (ret == 0 || ret == -1) {
                pfds[stdi].fd = -1;
            } else if (ret == -2) {
                pfds[stdi].events = POLLIN;
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
            ret = Net_recv_and_print(pfds[neti].fd);
            if (ret == 0 || ret == -1) {
                // On an orderly EOF, we nicely shutdown the socket.
                if (ret == 0) {
                    shutdown(pfds[neti].fd, SHUT_RD);
                }
                pfds[neti].fd = -1;
            } else if (ret == -2) {
                pfds[neti].events = POLLIN;
            }
        }

        // Write to the socket if possible and we have data in the buffer.
        if (pfds[neto].revents & POLLOUT && stdinpos > 0) {
            ret = Net_send(pfds[neto].fd, stdinbuf, stdinpos, cfg);
            if (ret == 0 || ret == -1) {
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
            // Feels weird to set this here, but this is the last place that
            // persistent configuration is available. The Net functions just
            // have a copy.
            if (cfg.send_creds > 0) {
                cfg.send_creds = 0;
            }
        }

        // If stdin is gone and buffer is empty, shutdown the network
        // connection.
        if (pfds[stdi].fd == -1 && stdinpos == 0) {
            if (pfds[neto].fd != -1) {
                shutdown(pfds[neto].fd, SHUT_WR);
            }
            pfds[neto].fd = -1;
        }

        // If the socket read is gone, so is socket write.
        if (pfds[neti].fd == -1) {
            pfds[neto].fd = -1;
        }
    }
}

static ssize_t Std_read(int fd, uint8_t *buf, size_t buflen) {
    ssize_t n = read(fd, buf, buflen);
    // Blocking and interruptions aren't errors, try again later.
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
        return -2;
    }
    if (n < 0) {
        perror("on read");
    }
    return n;
}
