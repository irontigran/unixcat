#ifndef FDSTATE_H
#define FDSTATE_H

#include <stdbool.h>

bool FdState_is_persistent(int fd);
void FdState_set_persistent(int fd);

#endif  // FDSTATE_H
