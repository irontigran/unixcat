#ifndef SECURITY_H
#define SECURITY_H

#include <sys/socket.h>

#include "options.h"

/* Functions to implement and variables to set for supporting receiving control
 * messages with an SELinux context.
 */

// The message that the --help option displays. Set to the empty string if you
// don't want a message.
extern char *Security_help_message;

/* Append security specific options to the existing OptBundle; set to a no-op
 * to not provide any options.
 *
 * Returns the new OptBundle.
 */
OptBundle Security_register_options(OptBundle existing);

/* Set the specified socket descriptor to receive the peer's SELinux security
 * context.
 *
 * Returns 0 on success, a negative number on error.
 */
int Security_turn_on_passsec(int fd);

/* Check if a given cmsg contains a security message or not.
 */
bool Security_is_context(int cmsg_level, int cmsg_type);

/* Print a control message containing a security context to stdout.  You should
 * be sure that this cmsg actually contains a security context.
 */
void Security_print_seccxt(struct cmsghdr *cmsg);

#endif  // SECURITY_H
