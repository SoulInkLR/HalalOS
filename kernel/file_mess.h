#ifndef FILE_MESS_H
#define FILE_MESS_H

#include "queue.h"
#include "mem.h"
#include "processus.h"
#include "horloge.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define NBQUEUE 10 //nombre maximum de files

extern link proc_activables;

int cour_pid;

typedef struct message{
  int val;
  struct message* suiv;
}message;
//structure des messages (FIFO)

//structure de la file de messages
typedef struct file_mess{
  int fid; //identifie la file
  int nb_msg; //nombre de message dans la file
  int capacite;//capacite de la file en message
  message* msg_tete ; //pointeur sur le premier message
  message* msg_queue; //pointe sur le dernier message
  link head_fp;
  link head_fv;

} file_mess;

//tableau des files de messages
file_mess tab_file_mess[NBQUEUE]; 
void init_tab_file_mess(void);

int pcreate(int count);
int pdelete(int fid);
int psend(int fid, int message);
int preceive(int fid,int *message);
int pcount(int fid, int *count);
int preset(int fid);

void enfiler_msg (int message, int fid);
int defiler_msg (int fid);
#endif

