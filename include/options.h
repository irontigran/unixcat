#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <sys/types.h>

/* Bundle together the shortopts and longopts so they don't drift independently
 * of each other.  Both fields can be passed straight into the relevant getopt
 * functions.
 */
typedef struct optbundle {
    char *shortopts;
    struct option *longopts;
} OptBundle;

/* Append new shortopts and longopts to the existing OptBundle and return the
 * new, extended OptBundle.
 *
 * If the provided shortopts or longopts are invalid, returns the existing
 * OptBundle with no changes.
 */
OptBundle Options_append(OptBundle existing, const char *newshorts,
                         const struct option *newlongs);

/* Convenience functions for converting a string to {pug}id_t.
 *
 * All return true on success, false on failure, and fill the provided pointers
 * with the results.
 */
bool atopid(const char *str, pid_t *result);
bool atouid(const char *str, uid_t *result);
bool atogid(const char *str, gid_t *result);

#endif  // OPTIONS_H
