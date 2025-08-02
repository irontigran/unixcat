#include "seqpacket.h"

/* seqpacket implementation when seqpackets aren't supported.
 */

char *Seqpacket_help_message = "";

OptBundle Seqpacket_register_options(OptBundle existing) {
    return existing;
}

