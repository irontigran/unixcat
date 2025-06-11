#include <stdio.h>

#include "config.h"
#include "creds.h"
#include "help.h"
#include "security.h"

void usage(const char *progname) {
    fprintf(stderr,
            "Usage: %s [OPTIONS] SOCKET\n"
            "Interact with unix domain sockets from the command line.\n\n"
            "Options:\n"
            "  -l, --listen         listen on the specified socket\n"
            "  -s, --source <path>  specify source address to use (doesn't "
            "affect -l)\n"
            "  -f, --fd <path>      pass a file descriptor corresponding to "
            "<path>\n"
            "%s"
            "%s"
            "  --help       display this help and exit\n"
            "  --version    output version information and exit\n\n"
            "Examples:\n"
            "  ucat /tmp/sock       -> connect to /tmp/sock\n"
            "  ucat -l /tmp/sock    -> listen on /tmp/sock\n"
            "  ucat -lR always /tmp/sock    -> receive credentials on every "
            "message\n"
            "  ucat -S once /tmp/sock       -> send credentials on only the "
            "first message\n"
            "  ucat --pid 1 /tmp/sock       -> pretend to be pid 1 when "
            "sending creds\n",
            progname, Creds_help_message, Security_help_message);
}

void version() { fprintf(stderr, "%s\n", VERSION); }
