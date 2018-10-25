#ifndef _HORLOGE_H_
#define _HORLOGE_H_

#include "cpu.h"
#include "debugger.h"
#include "segment.h"
#include <stdio.h>
#include <string.h>
#include "processus.h"


#define CLOCKFREQ 50         
#define QUARTZ 0x1234DD

int s, m, h, irq, compteur;
//compteur de seconde, minute et heure respectiveme
//compteur => pour l'initialisation des secondes
//vu que la frequence est à 50hz(20ms) il faut attendre que le compteur 
//atteigne 50 pour incrementer les secondes
void init_h();
void masque_IRQ (int num_IRQ, int masque);
void masque_IRQ_slave (int num_IRQ, int masque);
void init_traitant (int num_IT, void (*traitant)(void));
void frequence_horloge (void);
extern void handler_horloge ();
void clock_settings(unsigned long *quartz, unsigned long *ticks);
void traitant_horloge ();
unsigned long current_clock();
void wait_clock(unsigned long clock);
#endif
