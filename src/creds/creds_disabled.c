#include <stdbool.h>

#include "creds.h"
#include "main.h"
#include "options.h"

char *Creds_help_message = "";

OptBundle Creds_register_options(OptBundle existing) { return existing; }

int Creds_turn_on_once(int fd) { return 0; }

int Creds_turn_on_persistent(int fd) { return 0; }

int Creds_confirm_recv_settings(int fd) { return 0; }

bool Creds_is_credential(int cmsg_level, int cmsg_type) { return false; }

void Creds_print_credential(struct cmsghdr *cmsg) { return; }

size_t Creds_sizeof_send_struct(void) { return 0; }

void Creds_fill_cmsg(struct cmsghdr *cmsg, AncillaryCfg cfg) { return; }
