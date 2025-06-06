#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct optbundle {
    char *shortopts;
    struct option *longopts;
} OptBundle;

OptBundle Options_append(OptBundle existing, const char *newshorts,
                         const struct option *newlongs);

bool atopid(const char *str, pid_t *result);
bool atouid(const char *str, uid_t *result);
bool atogid(const char *str, gid_t *result);

#endif  // OPTIONS_H
