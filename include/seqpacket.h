#ifndef SEQPACKET_H
#define SEQPACKET_H

#include "options.h"

/* Functions to implement and variables to set for supporting the sequenced
 * packet protocol.
 */

// The message that the --help option displays. Set to the empty string if you
// don't want a message.
extern char *Seqpacket_help_message;

/* Append seqpacket options to the existing OptBundle; will just return the
 * existing OptBundle if seqpackets aren't supported.
 */
OptBundle Seqpacket_register_options(OptBundle existing);

#endif
