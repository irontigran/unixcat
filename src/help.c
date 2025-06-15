#include <stdio.h>

#include "config.h"
#include "creds.h"
#include "help.h"
#include "security.h"
#include "seqpacket.h"

void usage(const char *progname) {
    fprintf(stderr,
            "Usage: %s [OPTIONS] SOCKET\n"
            "Interact with unix domain sockets from the command line.\n\n"
            "Options:\n"
            "  -l, --listen         listen on the specified socket\n"
            "  -s, --source <path>  specify source address to use (doesn't "
            "affect -l)\n"
            "  -u, --udp            use datagram sockets instead of stream "
            "sockets\n"
            "%s"
            "  -f, --fd <path>      pass a file descriptor corresponding to "
            "<path>\n"
            "%s"
            "%s"
            "  --help       display this help and exit\n"
            "  --version    output version information and exit\n\n"
            "Examples:\n"
            "  ucat /tmp/sock       -> connect to /tmp/sock\n"
            "  ucat -l /tmp/sock    -> listen on /tmp/sock\n",
            progname, Seqpacket_help_message, Creds_help_message, Security_help_message);
}

void version() { fprintf(stderr, "%s\n", VERSION); }
