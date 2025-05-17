#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct optbundle {
    char *shortopts;
    struct option *longopts;
} OptBundle;

OptBundle Options_append(OptBundle existing, const char *newshorts,
                         const struct option *newlongs);

#endif  // OPTIONS_H
