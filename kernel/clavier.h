#ifndef __CLAVIER_H__
#define __CLAVIER_H__


#include "horloge.h"
#include "ecran.h"
#include "cpu.h"
#include "debugger.h"
#include "segment.h"
#include <stdio.h>
#include "kbd.h"
#include "processus.h"

#define _NB_SHELLS_ 4

int fid_clav[_NB_SHELLS_];

extern char buffer[];
extern int fin_buff;
extern int aff_ON;


int mess;

void init_buff(void);
void init_clav();
extern void handler_clavier ();
void traitant_clavier();
extern void kbd_leds(unsigned char leds);

void cons_echo(int on);
int cons_write(const char *str, long size);
unsigned long cons_read(char *string, unsigned long length);

extern void affiche_buff();
#endif
