#ifndef CREDS_H
#define CREDS_H

#include <stdbool.h>
#include <sys/socket.h>

#include "main.h"
#include "options.h"

/* Available credential passing functionality differs a lot between different
 * OSes.  The various credential functions should hide all of that complexity.
 * If you're supporting a new OSes credential passing, these are the functions
 * you need to implement.
 */

// The message that the --help option displays. Set to the empty string if you
// don't want a message.
extern char *Creds_help_message;

/* Append the credential specific options to the existing OptBundle (eventually
 * used to parse the CLI options); different OSes will have different versions
 * of this function to account for different functionality.
 */
OptBundle Creds_register_options(OptBundle existing);

/* Set the specified socket descriptor to receive a credential from its peer
 * exactly once.  Succeeding recvmsg calls will not have any credential
 * messages attached.
 *
 * Returns 0 on success, a negative number on error.
 */
int Creds_turn_on_once(int fd);

/* Set the specified socket descriptor to receive credentials from its peer in
 * perpetuity.  Every recvmsg call will have credential messages attached.
 *
 * Returns 0 on success, a negative number on error.
 */
int Creds_turn_on_persistent(int fd);

/* Check that the credential settings are still set correctly. This is a hack.
 * Ideally, the credential module would get it right the first time, but OSes
 * behave differently.  Some clear credential settings after recvmsg, some
 * don't. Call this function every time after a recvmsg call and the credential
 * module will do the right thing.
 *
 * Returns 0 on success, a negative number on error.
 */
int Creds_confirm_recv_settings(int fd);

/* Check if a given control message is a credential. Implementation hidden
 * because different OSes use different constants.
 */
bool Creds_is_credential(int cmsg_level, int cmsg_type);

/* Print a control message containing a credential to stdout in a human
 * readable way.  Each OS has a different struct with different fields. You
 * should be sure that this cmsg actually contains a credential message,
 * otherwise you will get gibberish.
 */
void Creds_print_credential(struct cmsghdr *cmsg);

/* Get the size of the struct you need to send a credential messages. Some OSes
 * disallow sending altogether, others use different structs.
 */
size_t Creds_sizeof_send_struct(void);

/* Fill a control message you're about to send with credential data based on
 * the provided configuration.  Only applies to OSes that allow sending
 * credentials. It's the caller's responsibility to check that the cmsg struct
 * has enough room to hold a credential message.
 */
void Creds_fill_cmsg(struct cmsghdr *cmsg, AncillaryCfg cfg);

#endif  // CREDS_H
