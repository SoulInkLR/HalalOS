#ifndef _PROCESSUS_H_
#define _PROCESSUS_H_

#include "hash.h"
#include "userspace_apps.h"
#include "cpu.h"
#include "debugger.h"
#include "segment.h"
#include "horloge.h"
#include "mem.h"
#include "processor_structs.h"
#include "retour_en_user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "allocateur.h"
#include "mapping.h"
#include "file_mess.h"
#include "clavier.h"
#include "ecran.h"

#define _MAX_PRIO_ 100000         //priorite max
#define NBPROC 300           //nombre de processus total
#define _TAILLE_ZONE_ 6    //taille du tableau stockant le context
#define _TAILLE_PILE_ 4096 //taille de la pile (16ko)


int s, m, h, compteur;


struct display_switcher;

enum Status { actif,
	      bloque_sur_file_pleine,
	      bloque_sur_file_vide,
	      activable, 
	      endormi,
	      zombie,
	      vide,
	      bloque_en_attente_fils};

typedef struct PS {

  int pid;                  // pid du ps
  char *nom;                // nom du processus
  unsigned long duree;
  int display;
  int priorite;             // niveau de priorite
  int pid_pere;
  enum Status stat;         // actif, bloqué, activable ou endormi
  int retval;               // valeur de retour du processus (-1 par défaut changé par un exit)
  int *retval_fils;
  int save [_TAILLE_ZONE_]; // zone de sauvgarde du contexte 
  int *pile_user;           // pile du user 
  int *pile_kernel;         // pile kernel 
  int waitpid;              // Si proc est bloque en attente de fils:  pid du processus attendu (ou -1)
  link proc_fp;             //lien pour les files de messages pleine
  link proc_fv;             //lien pour les files de messages vide
  int msg;                  //message a deposer ou retirer de la file
  int has_received_msg;     //champ à mettre à 1 lorsqu'un processus bloqué en file vide a été débloqué par un psender (en // son champ msg prend la valeur du message envoyé)
  int fid_bloque;           //indice de la file de message ou le ps est bloque
  link link_proc_activables;
  link link_fils;
  link link_pere;
  
} PS;


PS tab_ps [NBPROC];     
extern int cour_pid; //pointeur sur les processus
extern int old_display_pid;

int start(const char *name, unsigned long ssize, int prio, void *arg);
int getpid (void);
int getprio (int pid);
int chprio(int pid, int newprio);
int kill (int pid);
int waitpid(int pid, int *retvalp);
void exit(int retval);

void ordonnance (void);
extern void ctx_sw (int *old, int *new );
extern void handler_page_fault (void);

void map_code (unsigned long* cr3, unsigned long* adr_start, unsigned long* adr_end);
void init_tab_apps(void);
char* enum_to_str(enum Status status);
void free_proc(int pid);
void delinker_les_fils (int pid);
int get_free_pid(void);
int get_time (void);
void display_banner (void);
void funct_ps(int i);
void funct_sinfo(int i);
void free_user (int pid);

#endif

