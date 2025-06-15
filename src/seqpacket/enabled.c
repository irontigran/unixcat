#include <getopt.h>
#include <stddef.h>

#include "options.h"
#include "seqpacket.h"

char *Seqpacket_help_message = "      --seq            use sequenced-packet sockets\n";

OptBundle Seqpacket_register_options(OptBundle existing) {
    const struct option longopts[] = {
        (struct option){
            .name = "seq", .has_arg = no_argument, .flag = NULL, .val = 0},
        (struct option){.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}};
    return Options_append(existing, "", longopts);
}
