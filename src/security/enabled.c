#include <getopt.h>
#include <stddef.h>

#include "options.h"
#include "security.h"

char *Security_help_message =
    "  --security   send the SELinux security context\n";

OptBundle Security_register_options(OptBundle existing) {
    const struct option longopts[] = {
        (struct option){
            .name = "security", .has_arg = no_argument, .flag = NULL, .val = 0},
        (struct option){.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}};
    return Options_append(existing, "", longopts);
}
