#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

OptBundle Options_append(OptBundle existing, const char *newshorts,
                         const struct option *newlongs) {
    assert(newshorts != NULL);
    assert(newlongs != NULL);

    int old_shortlen = 0;
    if (existing.shortopts != NULL) {
        old_shortlen = strlen(existing.shortopts);
        // Sanity checking: we shouldn't be registering any options twice, so no
        // repeated arguments in the short options.
        for (int i = 0; i < strlen(newshorts); i++) {
            if (newshorts[i] != ':' &&
                strchr(existing.shortopts, newshorts[i]) != NULL) {
                return existing;
            }
        }
    }

    int old_longlen = 0;
    if (existing.longopts != NULL) {
        for (; existing.longopts[old_longlen].name != NULL; old_longlen++) {
            ;
        }
    }
    int new_longlen = 0;
    for (; newlongs[new_longlen].name != NULL; new_longlen++) {
        ;
    }

    int tot_shortlen = old_shortlen + strlen(newshorts);
    int tot_longlen = old_longlen + new_longlen;

    char *shorttmp = realloc(existing.shortopts, tot_shortlen + 1);
    if (shorttmp == NULL) {
        return existing;
    }
    struct option *longtmp =
        realloc(existing.longopts, sizeof(struct option) * (tot_longlen + 1));
    if (longtmp == NULL) {
        return existing;
    }
    existing.shortopts = shorttmp;
    existing.longopts = longtmp;

    strcpy(existing.shortopts + old_shortlen, newshorts);
    existing.shortopts[tot_shortlen] = '\0';

    memcpy(existing.longopts + old_longlen, newlongs,
           sizeof(struct option) * new_longlen);
    existing.longopts[tot_longlen] =
        (struct option){.name = NULL, .has_arg = 0, .flag = NULL, .val = 0};

    return existing;
}
