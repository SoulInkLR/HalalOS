#ifndef _MAPPING_H_
#define _MAPPING_H_

#include "processus.h"
#include "allocateur.h"
#include <stdio.h>


void map(unsigned long* cr3, unsigned long* adr_v, unsigned long* adr_ph, unsigned long permission);
void copy_entry (void* ptr);
void init_cr3_idle (void* ptr);
void init_cr3 (void* ptr_cr3);




#endif
