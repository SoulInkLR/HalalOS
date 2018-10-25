#ifndef _SHELL_H_
#define _SHELL_H_
#include "syscalls.h"
#include <string.h>
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

#define _TAILLE_BUFF_ 20

void trait_cmd (void);
void parsecmd (char* cmd,
	       unsigned long taille_cmd,
	       int* num,
	       int* arg1);
void exec (int num, int arg1);

#endif
