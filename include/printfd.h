#ifndef PRINTFD_H
#define PRINTFD_H

/* Print a human-readable description of a file descriptor.
 *
 * The output will vary depending on what functionality is available on the OS.
 * In the best case scenario, there is some facility that allows us to print a
 * path corresponding to the fd.  In the worst case, we have to print a
 * category, like [socket], or [regular file].
 */
void PFD_print_fd(int fd);

#endif  // PRINTFD_H
