#ifndef _FCT_KERNEL_
#define _FCT_KERNEL_
#include "processus.h"
#include "clavier.h"
#include "shared_mem.h"
#include "ecran.h"

int select_call (int num, int arg1, int arg2, int arg3, int arg4);
void handler_syscall (void);

#endif
