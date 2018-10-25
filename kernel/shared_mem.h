#ifndef _SHARED_MEM_
#define _SHARED_MEM_

#include "processus.h"
#include <string.h>
#include "allocateur.h"
#include "mem.h"

#define NB_PP 16

typedef struct shared_page{
  void *page;
  void *virt;
  char *key;
  int ref;

} shared_page;


//liste des pages physiques
shared_page liste_pp_noyeau[NB_PP];

void *shm_create(const char *k);
void *shm_acquire(const char *k);
void shm_release(const char *k);

shared_page* recherche_page(const char *k);
shared_page* get_page_free();

void *add_sp(const char *k);

int del_sp(const char *k);

void init_virt();

int egal(const char *s1, const char *s2);

#endif
