#include "shared_mem.h"



/* ----------------fonction sur le partage de la memoire--------------*/

//fonction qui permet au processus appelant de demander la création d'une page partagée au noyau
void *shm_create(const char *k){
  
  if(k == NULL)
    return NULL;
  
  shared_page *p;
  p = recherche_page(k);

  if(p!=NULL) //la page existe deja
    return NULL;
  else{     
    return add_sp(k);
  }
 
}

//fonction qui permet à un processus d'obtenir une référence sur une page partagée connue du noyau sous le nom key
void *shm_acquire(const char *k){
  
  if(k == NULL)
    return NULL;

  shared_page *p = recherche_page(k);
  if(p!=NULL){
    void *av = p->virt;

    map((unsigned long*)tab_ps[cour_pid].save[5],av,p->page,7);
    p->ref ++;
    return p->virt;
  }
  return NULL;
}

//le processus appelant veut relacher la référence sur la page nommée par key
void shm_release(const char *k){
 
  if(k == NULL)
    return;

  shared_page *p = recherche_page(k);
  //si la page a ete mappe
  if(p!=NULL){ 
    void *av = p->virt;
    //demapper
    map((unsigned long*)tab_ps[cour_pid].save[5],av,p->page,0);

    p->ref --;
    if(p->ref == 0){
      del_sp(k);
    }
  }
}


/* ----------------fonction sur les pages partagees------------------*/

//fonction qui recherche une page et retourne le pointeur vers celle ci si elle existe, NULL sinon. 
shared_page *recherche_page(const char *k){
  int i = 0;
  int eg = egal(k,(liste_pp_noyeau[i].key));

  while ( i< NB_PP){
    if(!eg){
      i++;
      eg = egal(k,(liste_pp_noyeau[i].key));
    }
    else{
      return liste_pp_noyeau+i ;
    }
  }
 
  return NULL;
}

shared_page *get_page_free(){
 
  int i =0;

   while ( i < NB_PP){
     if(liste_pp_noyeau[i].page == NULL){
        return &liste_pp_noyeau[i];
     }
     else 
       i++;
   }

   return NULL;
}

//fonction qui ajoute la sp a la liste des pages partagees avec le noyeau
void* add_sp(const char *k){
  
  shared_page *p =get_page_free();
 
  p->page = alloc_user_page();    
 
  p->key = mem_alloc(strlen(k)+1);
  strcpy (p->key,k);

  p->ref = 1;
  map((unsigned long*)tab_ps[cour_pid].save[5],p->virt,p->page,7);
  
  return p->virt;
}



//fonction qui elimine la sp de la liste des pages partagees avec le noyeau
int del_sp(const char *k){
  
  shared_page *pp = recherche_page(k);
  // si la page n'existe pas
  if( pp == NULL)
    return 0;
  //sinon
  else{
    if(pp->page != NULL){
      free_user_page(pp->page);
    }
    
    mem_free(pp->key, strlen(pp->key)+1);
    return 1;
    }
}

/* ----------------fonction sur les adresses virtuelles--------------*/
  
// fonction qui retourne 1 si s1 est egale a s2, 0 sinon
int egal(const char *s1, const char *s2){
  if ((s2 == NULL) || (s1 == NULL))
    return 0;
  if (strcmp(s1, s2)==0)
    return 1;
  else
    return 0;
}
	
void init_virt(){
  int i=1;
  liste_pp_noyeau[0].virt = (unsigned long *)0xC0001000;
  while (i < NB_PP){
     liste_pp_noyeau[i].virt = liste_pp_noyeau[i-1].virt + 0x1000;
    
     i++;
  }
}
