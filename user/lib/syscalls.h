#ifndef _SYSCALL_H_
#define _SYSCALL_H

//fonctions pour les processus

int start(const char *name, unsigned long ssize, int prio, void *arg);
int getpid (void);
int getprio (int pid);
int chprio(int pid, int newprio);
int kill (int pid);
int waitpid(int pid, int *retvalp);
void exit(int retval);


//fonctions pour avoir le temps courant

unsigned long current_clock (void);
void clock_settings(unsigned long *quartz, unsigned long *ticks);
void wait_clock(unsigned long clock);

//fonction pour la manipulation des filles de messages

int pcreate(int count);
int pdelete(int fid);
int psend(int fid, int message);
int preceive(int fid,int *message);
int pcount(int fid, int *count);
int preset(int fid);


// Fonction d'affichage
void cons_write (const char *str, int size);
void cons_echo (int on);
unsigned long cons_read (char* string, unsigned long length);

//Fonction partage memoire
void *shm_create (const char *k);
void *shm_acquire (const char *k);
void shm_release (const char *k);

//fonction shell
void efface_ecran (void);
void funct_ps(int i);
void funct_sinfo(int i);
void display_banner();
void switch_display(int target_pid);

#endif
