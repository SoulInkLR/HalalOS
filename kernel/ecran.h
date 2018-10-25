#ifndef _ECRAN_H_
#define _ECRAN_H_

#include "cpu.h"
#include "debugger.h"
#include "processus.h"
#include <stdio.h>
#include <string.h>

#define _NB_SHELLS_ 4

extern int ligne;
extern int colonne;
extern int old_display_pid;

struct display_switcher {
  short * old_display;
  int ligne, colonne;
  int alloue;
};
extern struct display_switcher display_sw[_NB_SHELLS_];

typedef struct prec_char {
  char c;
  int prio;
  link l_prec_char;
} prec_char;

void init_lig_col_display(int display_pid);
short *ptr_mem(int lig, int col);
void ecrit_car(int lig,int col,unsigned short coul,unsigned short fond,
	       unsigned short clig, char c);
void place_curseur(int lig, int col);
void efface_ecran(void);
void traite_car(char c);
void defilement(void);
void console_putbytes(const char *chaine, int taille);
void switch_display (int target_pid);
#endif
