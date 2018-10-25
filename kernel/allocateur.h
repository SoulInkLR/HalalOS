#ifndef _ALLOCATEUR_H_
#define _ALLOCATEUR_H_

#include "mem.h"
#include "horloge.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
extern int cmp;

void mem_init(void);
void* alloc_user_page (void);
void free_user_page (void* ptr);
void init_zone (void* ptr, unsigned long size);

#endif
