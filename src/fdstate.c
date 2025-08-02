#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "fdstate.h"

// A simple table, indexed by fd, storing whether a given fd is supposed to
// receive credentials persistently or non-persistently. Should not be used
// outside this module.
static bool *g_persistence_tab = NULL;
// The maximum number of fds the table can currently store.
static int g_maxfds = 0;

/* Expand the table so it can store the persistence or not of the specified fd.
 *
 * Returns 0 on success, -1 on failure.
 */
static int expand_tab(int fd) {
    size_t newsize = MAX(g_maxfds, fd);
    bool *tmp = realloc(g_persistence_tab, newsize * sizeof(bool));
    if (tmp == NULL) {
        return -1;
    }
    g_persistence_tab = tmp;
    memset(g_persistence_tab + g_maxfds, false,
           (newsize - g_maxfds) * sizeof(bool));
    g_maxfds = newsize;
    return 0;
}

bool FdState_is_persistent(int fd) {
    if (fd < g_maxfds) {
        return g_persistence_tab[fd];
    }
    if (expand_tab(fd) < 0) {
        return false;
    }
    return g_persistence_tab[fd];
}

void FdState_set_persistent(int fd) {
    if (fd < g_maxfds) {
        g_persistence_tab[fd] = true;
    }
    if (expand_tab(fd) < 0) {
        return;
    }
    g_persistence_tab[fd] = true;
}
