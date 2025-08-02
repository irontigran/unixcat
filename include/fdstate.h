#ifndef FDSTATE_H
#define FDSTATE_H

#include <stdbool.h>

/* This module provides a simple way to check if we wanted a socket file
 * descriptor to receive credentials with every message (persistently) or just
 * once (non-persistently).
 *
 * This is just storage---it does not interact with the OS.
 */

/* Check if we previously set the socket fd to receive creds persistently.
 */
bool FdState_is_persistent(int fd);

/* Store that we want the specified socket fd to receive creds persistently.
 */
void FdState_set_persistent(int fd);

#endif  // FDSTATE_H
