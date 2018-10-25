#include "processus.h"
 
int nbproc = 1;
PS* proc_a_liberer = NULL;
struct x86_tss tss;
link proc_activables;
LIST_HEAD(proc_activables);
hash_t* tab_apps;

void init_tab_apps (void){
  int i = 0;
  while (symbols_table[i].name != NULL){
    i++;
  }
  tab_apps = mem_alloc (i*sizeof(hash_t));
  hash_init_string (tab_apps);
  i = 0;
  while (symbols_table[i].name != NULL){
    hash_set (tab_apps, (void*)symbols_table[i].name,(void*) &symbols_table[i]);
    i++;
  }
  
}
void free_user (int pid){
  unsigned long* pd = (unsigned long*) tab_ps [pid].save[5];
  unsigned long* pt;
  unsigned long* adr_ph;
  
  for (int i = 64; i < 768; i++){
    if ( *(pd+i) != 0){
      pt = (unsigned long*)(*(pd+i) & ~0xFFF);
      for (int j = 0; j< 1024; j++){
	adr_ph = (unsigned long*)(*(pt+j) & ~0xFFF);
	if (adr_ph !=0){
	   free_user_page (adr_ph);
	}
      }
      free_user_page (pt);    
    }
  }
  free_user_page (pd);
}

void map_code (unsigned long* cr3,
	       unsigned long* adr_start,
	       unsigned long* adr_end){
  int k =0;
  while((unsigned long)adr_start+k*0x1000<(unsigned long)adr_end){
    k++;
  }

  //copie du code
  unsigned long* temp;
  for (int i=0;i<k; i++){
    temp = alloc_user_page ();
    map (cr3,(unsigned long*) (0x40000000+i*0x1000), (unsigned long*)temp, 7);
    memcpy (temp, adr_start, 4096);
    adr_start =  (unsigned long*)((unsigned long) (adr_start)+0x1000);
  }
}
// retourne le temps qu'affiche l'horloge en seconde
int get_time (){
  return s+m*60+h*3600;
}


//retourn l'index d'une case libre dans tab_ps 
int get_free_pid(void){
  if (nbproc > NBPROC) return -1; //pas de case libre
  int i = 0;
  while(i<NBPROC){
    if (tab_ps[i].stat == vide) return i;
    i++;
  }
  return -1; //return bidon
}


char* enum_to_str(enum Status status) {
  switch (status) {
  case actif : 
    return "actif";
    break;
  case bloque_sur_file_pleine : 
    return "bloque_sur_file_pleine";
    break;
  case bloque_sur_file_vide : 
    return "bloque_sur_file_vide";
    break;
  case activable : 
    return "activable";
    break;
  case endormi : 
    return "endormi";
    break;
  case zombie : 
    return "zombie";
    break;
  case vide : 
    return "vide";
    break;
  case bloque_en_attente_fils : 
    return "bloque_en_attente_fils";
    break;
  default:
    return "inconnu";
  }
}

// On libère le pid du processus dans le tableau des processus

void free_proc(int pid) {
  
  nbproc--;
  tab_ps[pid].pid_pere = -1;
  tab_ps[pid].stat = vide;
  tab_ps[pid].retval = 0;
  int *pile = tab_ps[pid].pile_kernel;
  int ssize = tab_ps[pid].pile_kernel[-2];
  mem_free (pile, ssize);
  mem_free (tab_ps[pid].nom, strlen(tab_ps[pid].nom)+1);
  free_user (pid);

}



//fonction d'ordonnancement, critères : 
//     ** tourniquet
//     ** écheance du temps d'endormissement
void ordonnance(void){
   PS* p1 = NULL;
   int i = 0;
   queue_for_each (p1, &proc_activables, PS, link_proc_activables) {
    i++;
   } 
 
  // On passe en activable les processus qui sont endormis et devraient se réveiller
  for (int i = 0 ; i < NBPROC ; i++) {
    if (tab_ps[i].stat == endormi && tab_ps[i].duree < current_clock()) {
      tab_ps[i].stat = activable;
      queue_add(&tab_ps[i], &proc_activables, PS, link_proc_activables, priorite);
    }  
  }
  
  // Ici on libère le processus précédent dans l'ordonnancement qui n'a pas pu être entièrement libéré 
  // (mem_free) car sa pile était alors lu par le registre EIP.
  // Comme il est terminé et qu'il a été marqué vide, on le free_proc
  if (proc_a_liberer != NULL && proc_a_liberer->stat == vide && proc_a_liberer->pid != cour_pid) {
    free_proc(proc_a_liberer->pid);
    proc_a_liberer = NULL;
  }
  
  // Si la liste est vide, on ne fait rien dans l'ordonnancement (idle actif)
  if (queue_empty(&proc_activables))
    return;

  // On vérifie la priorité du prochain processus à activer dans les proc_activables
  PS *p = queue_top(&proc_activables, PS, link_proc_activables);
  // Si cette priorité est inférieure à celle du processus courant et que celui-ci
  // est actif, on ne fait rien dans l'ordonnancement
  if (tab_ps[cour_pid].priorite > p->priorite && tab_ps[cour_pid].stat == actif)
    return;

  // Le processus courant va être préempté
  // S'il était actif, il passe en activable et 
  //on l'ajoute dans les proc_activables
  if (tab_ps[cour_pid].stat == actif) {
    tab_ps[cour_pid].stat = activable;
    queue_add(&tab_ps[cour_pid], &proc_activables, PS, link_proc_activables, priorite); 
  }

  // On récupère le pid du processus à activer
  int old_pid = cour_pid;
  cour_pid = ((PS*)queue_out(&proc_activables,
			     PS, link_proc_activables))->pid;
  tab_ps[cour_pid].stat = actif;
  
  if (old_pid != cour_pid){
    tss.esp0 = (int) &(tab_ps[cour_pid].pile_kernel [_TAILLE_PILE_ / sizeof(int)-1]);
    ctx_sw(& (tab_ps[old_pid].save[0]),&(tab_ps[cour_pid].save[0]));
  } 
}


int start(const char *name, unsigned long ssize, int prio, void *arg){
  //cherche si il y'a un espace processus libre
  struct uapps* code = hash_get (tab_apps,(void*) name, NULL); 
  int i = get_free_pid();
  if (i == -1){
    printf("nombre max de processus atteint \n");
    return -1;
  }
  
  void* adr_start = code->start;
  void* adr_end = code->end;
  
  unsigned long* ptr_pile;
  int taille_pile = (int)ssize;
  int t = 0;
  
  
  // Gestion du display du processus
  // si on n'a pas encore initialisé de display
  int courd = 0; 
  while (courd < _NB_SHELLS_ && display_sw[courd].alloue != 0) {
    courd++;
  }
  // Si on alloue un affichage pour la premiere fois
  if (courd != _NB_SHELLS_) {
    tab_ps[i].display = courd;
    display_sw[courd].alloue = 1;
    display_sw[courd].old_display = mem_alloc(4000);
    memset(display_sw[courd].old_display,0, 4000);
    display_sw[courd].ligne = display_sw[courd].colonne = 0;
      

  }
  // sinon on hérite de l'affichage du père 

  else 
    tab_ps[i].display = tab_ps[cour_pid].display;
 

  tab_ps[i].nom = mem_alloc (strlen(name)+1);
  strcpy (tab_ps[i].nom, name);
  //1.pile kernel
  tab_ps[i].pile_kernel = mem_alloc (_TAILLE_PILE_);
  //2.page directory, initialisation de PD et mapping du code
  tab_ps[i].save[5] = (unsigned long) alloc_user_page ();
  init_cr3 ((void*)tab_ps[i].save[5]);
  
  if ((ssize / 4096) > 10)  return -1;
  if (taille_pile == 0){
    ptr_pile = alloc_user_page ();
    tab_ps [i].pile_user = (int*)ptr_pile;
    map ((unsigned long*)tab_ps[i].save[5], (unsigned long*)(0xC0000000-t*0x1000), ptr_pile, 7); 
  }else{
    while ( taille_pile > 0){
      ptr_pile = alloc_user_page ();
      if (t == 0) tab_ps [i].pile_user = (int*)ptr_pile;
      map ((unsigned long*)tab_ps[i].save[5], (unsigned long*)(0xC0000000-t*0x1000), ptr_pile, 7);
      t++;
      taille_pile = taille_pile - 4096;
    }
  }

  map_code ((unsigned long*)tab_ps[i].save[5], (unsigned long*) adr_start,
	    (unsigned long*) adr_end);
  //adresage
  tab_ps[i].save[0]=(int) &(tab_ps[i].pile_kernel [_TAILLE_PILE_ / sizeof(int)-6]); 
  //case correspondante a %esp
 
  tab_ps[i].pile_kernel[_TAILLE_PILE_/(sizeof(int))-1]=(int)(USER_DS);//%ss 
  tab_ps[i].pile_kernel[_TAILLE_PILE_/(sizeof(int))-2]=(0xC0000FF8);//%esp 
  tab_ps[i].pile_kernel[_TAILLE_PILE_/(sizeof(int))-3]=(int)(0x202); //%eflags
  tab_ps[i].pile_kernel[_TAILLE_PILE_/(sizeof(int))-4]=(int)(USER_CS);//%cs
  tab_ps[i].pile_kernel[_TAILLE_PILE_/(sizeof(int))-5]=(int)(0x40000000);//%eip 
  tab_ps[i].pile_kernel[_TAILLE_PILE_/(sizeof(int))-6]=(int)(retour_en_user);//iret 

  //Fausse adresse de retour jamais utilisée par le compilo
  //contient le code du processus
  tab_ps[i].pile_user[4096/(sizeof(int))-2]=0x67892345;
  tab_ps[i].pile_user[4096/(sizeof(int))-1]=(int)arg;
  
  if (arg == NULL) {} 
  tab_ps[i].pid = i;
  tab_ps[i].pid_pere = cour_pid;
  tab_ps[i].retval = 0;
  tab_ps[i].retval_fils = 0;
  tab_ps[i].stat = activable;
  tab_ps[i].priorite = prio;
  tab_ps[i].stat = activable;
  queue_add (&tab_ps[i], &proc_activables, PS, 
	     link_proc_activables, priorite);
  tab_ps[i].duree = 0;  
  //ajout du processus dans la liste du pere
  INIT_LIST_HEAD (&(tab_ps[i].link_fils));
  queue_add (&tab_ps[i], &tab_ps[cour_pid].link_fils, PS, link_pere, priorite);  
  //si priorite du fils plus grande que le père l'active
  if (tab_ps[cour_pid].priorite <= prio){
    ordonnance();
  }
 
    nbproc++;
    return i;
}

// retourne le pid du processus courant
int getpid (void){
  return tab_ps[cour_pid].pid;
}

//retourne la priorite du processus courant
int getprio (int pid){
  if (pid<0 || pid>=NBPROC){
    printf("pid invalid \n");
    return -1;}
  if (tab_ps[pid].pid!=pid){
    printf("processus déja mort \n");
    return -1;
  }
  return tab_ps[pid].priorite;
}

int waitpid(int pid, int *retvalp){
  PS* fils = NULL;
 if ((retvalp < (int*)0x4000000) && (retvalp != NULL))
    return 0;
  
   // Vérification de la validité du waitpid
  if (queue_empty(&tab_ps[cour_pid].link_fils) || pid > NBPROC - 1 || pid == 0 || pid == cour_pid){
    return -1;
  }
  // Si le processus attend la terminaison de n'importe lequel de ses fils
  if (!queue_empty (&tab_ps[cour_pid].link_fils)){
    if (pid < 0){
      
      queue_for_each (fils, &tab_ps[cour_pid].link_fils, PS, link_pere){
	if (fils->stat == zombie){
	  queue_del(fils, link_pere);

	  if (retvalp != NULL) 
	    *retvalp = fils->retval;
	  	  
	  free_proc (fils->pid);
	  return fils->pid;
	  
	}
      }
      
      // Pas de fils encore termine
      tab_ps[cour_pid].stat = bloque_en_attente_fils;
      tab_ps[cour_pid].waitpid = -1;
    }
    else {
   
      queue_for_each (fils, &tab_ps[cour_pid].link_fils, PS, link_pere){
	if ((fils->pid == pid) && (fils->stat == zombie)){
	  queue_del(fils, link_pere);

	  if (retvalp != NULL){ 
	    *retvalp = fils->retval;
	  }
	  free_proc (fils->pid);
	  return fils->pid;
	  
	}
      }
      // Le fils attendu n'est pas encore terminé
      tab_ps[cour_pid].stat = bloque_en_attente_fils;
      tab_ps[cour_pid].waitpid = pid;
    }
  }else{
    return -1;
  }
  
  ordonnance();    
  
  // On est revenu d'ordonnance donc le processus récupère la valeur de retour de son fils terminé
  if (tab_ps[tab_ps[cour_pid].waitpid].stat == zombie) {
    if (retvalp != NULL) 
      *retvalp = tab_ps[tab_ps[cour_pid].waitpid].retval;
  
    queue_del(&tab_ps[tab_ps[cour_pid].waitpid], link_pere);
    free_proc(tab_ps[cour_pid].waitpid);
  }

  return tab_ps[cour_pid].waitpid;
}

  

void delinker_les_fils (int pid) {
  PS* p_fils = NULL;
  
  while (!queue_empty(&tab_ps[pid].link_fils)) {
    p_fils = queue_out(&tab_ps[pid].link_fils, PS, link_pere);
  
    if (p_fils->stat == zombie) {
      free_proc(p_fils->pid);
    }
    
    p_fils->pid_pere = -1;
    
  }
}

int kill(int pid) {
  
  // On vérifie la validité de l'appel de kill
  if (tab_ps[pid].stat == zombie || tab_ps[pid].stat == vide) 
    return -1;
  if (pid < 1 || pid > NBPROC - 1) 
    return -1;
  
  // On le retire de la liste des proc activables
  if (tab_ps[pid].stat == activable) 
    queue_del(&tab_ps[pid], link_proc_activables);

  // Si le processus a des fils 
  if (!queue_empty(&tab_ps[pid].link_fils)) {
    
    // Le processus va être terminé donc on supprime donc on marque ses fils comme
    // n'ayant plus de père et les fils zombies sont détruits
    delinker_les_fils(pid);
  }
  
  // On retire le proc d'une file si il est bloqué dedans
    if (tab_ps[pid].stat == bloque_sur_file_pleine)
      queue_del (&tab_ps[pid], proc_fp);
    else if (tab_ps[pid].stat == bloque_sur_file_vide)
       queue_del (&tab_ps[pid], proc_fv);
  
  // Si le processus a un père il devient zombie
  if (tab_ps[pid].pid_pere != -1) {
    int pid_pere = tab_ps[pid].pid_pere;
    tab_ps[pid].stat = zombie;
    
    // Si le père attend la fin d'un de ses fils
    if (tab_ps[pid_pere].stat == bloque_en_attente_fils) {
      // On vérifie qu'il attendait n'importe quel proc fils ou seulement celui demandé
      if (tab_ps[pid_pere].waitpid == -1 || tab_ps[pid_pere].waitpid == pid) {

	// On réactive le père qui était bloqué en attente d'un de ses fils et on lui passe 
	// la valeur de retour de son fils qui s'est terminé.
	tab_ps[pid].retval = 0;
      
	tab_ps[pid_pere].stat = activable;
	tab_ps[pid_pere].waitpid = pid;
	queue_add (&tab_ps[pid_pere], &proc_activables, PS, link_proc_activables, priorite);
      
      }
    }
  }
  // Il n'a pas de père donc on le free tout simplement
  else {
    tab_ps[pid].stat = vide;
    proc_a_liberer = &tab_ps[pid];
  }

  ordonnance();
 
  return 0;
}


void exit(int retval) {
  
  // Si le processus a des fils 
  if (!queue_empty(&tab_ps[cour_pid].link_fils)) {
    
    // Le processus va être terminé donc on supprime donc on marque ses fils comme
    // n'ayant plus de père et les fils zombies sont détruits
    delinker_les_fils(cour_pid);
  }
   
  // Si le processus a un père il devient zombie
  if (tab_ps[cour_pid].pid_pere != -1) {
    int pid_pere = tab_ps[cour_pid].pid_pere;
    tab_ps[cour_pid].stat = zombie;
    tab_ps[cour_pid].retval = retval;
    // Si le père attend la fin d'un de ses fils
    if (tab_ps[pid_pere].stat == bloque_en_attente_fils) {
      // On vérifie qu'il attendait n'importe quel proc fils ou seulement celui courant
      if (tab_ps[pid_pere].waitpid == -1 || tab_ps[pid_pere].waitpid == cour_pid) {
	
	// On réactive le père qui était bloqué en attente d'un de ses fils et on lui passe 
	// la valeur de retour de son fils qui s'est terminé.

	tab_ps[pid_pere].stat = activable;
	tab_ps[pid_pere].waitpid = cour_pid;
	queue_add (&tab_ps[pid_pere], &proc_activables, PS, link_proc_activables, priorite);
      
      }
    }
  }
  // Il n'a pas de père donc on le free
  else {
    tab_ps[cour_pid].stat = vide;
    proc_a_liberer = &tab_ps[cour_pid];
  }

  ordonnance();
  // On n'arrive jamais ici normalement
  assert(0);
}



int chprio (int pid, int newprio){
  
  if (pid<0 || pid>=NBPROC){
    printf("pid invalid \n");
    return -1;
  }
  if ((newprio <=0) || (newprio>_MAX_PRIO_)){
    printf("newprio invalid \n");
    return -1;
  }
  if ((tab_ps[pid].stat == zombie) || (tab_ps[pid].stat == vide)){
    printf("processus deja mort ou vide\n");
    return -1;
  }
 
  int oldprio = tab_ps[pid].priorite;
  tab_ps[pid].priorite = newprio;
  if (tab_ps[pid].stat == bloque_sur_file_pleine){
    queue_del (&tab_ps[pid], proc_fp);
    queue_add (&tab_ps[pid], &tab_file_mess[tab_ps[pid].fid_bloque].head_fp,
	       PS, proc_fp, priorite );
  }else if (tab_ps[pid].stat == bloque_sur_file_vide){
    queue_del (&tab_ps[pid], proc_fv);
    queue_add (&tab_ps[pid], &tab_file_mess[tab_ps[pid].fid_bloque].head_fv,
	       PS, proc_fv, priorite);
  } else if (tab_ps[pid].stat == activable){
    queue_del (&tab_ps[pid], link_proc_activables);
    queue_add (&tab_ps[pid], &proc_activables,
	       PS, link_proc_activables, priorite);
  } 
  
  ordonnance();
  
  return oldprio;
}




void traitant_page_fault(){
  
  printf (" Page fault");
  exit(0);

}

void funct_sinfo(int i){
 int *c = mem_alloc(sizeof(int));
  while(tab_file_mess[i].fid != -1 && i < NBPROC ){
    printf("%d\t%d\t%d\t%d\n", tab_file_mess[i].fid,
	   tab_file_mess[i].nb_msg, tab_file_mess[i].capacite,
	   pcount(tab_file_mess[i].fid,c));
    i++;
  }
  mem_free(c, sizeof(int));
}

void funct_ps (int i){
  while(&tab_ps[i] != NULL && i < NBPROC){
    if (tab_ps[i].stat != vide) {
      printf("%d\t%s\t%s\t\n", tab_ps[i].pid,
	     enum_to_str(tab_ps[i].stat), tab_ps[i].nom);
    }
    i++;
  }
}

void display_banner (void){

  printf("                     ++++++     ------     *******\n");
  printf("              +++++++++++  ----------------  ***********\n");
  printf("         ++++++++++++  --------------------------  ***********\n");
  printf("      ++++++++++++   ------------------------------   *************\n");
  printf("  +++++++++++  _   _  ______   _     ______   _   _____   ___  ************\n");
  printf(" +++++++++++  | |_| ||  __  | | |   |  __  | | | /  _  \\ / __|   ************\n");
  printf("+++++++++++   |  _  || |__| | | |__ | |__| | | ||_ |_|  |\\__\\     ************\n");
  printf("$$$$$$$$$$$   |_| |_|| |  | |_|____||_|  |_|_|____|____/ |__ /    !!!!!!!!!!!!\n");
  printf(" $$$$$$$$$$$                                                  !!!!!!!!!!!!!\n");
  printf("   $$$$$$$$$$$$$     --------------------------------      !!!!!!!!!!!!!\n");
  printf("         $$$$$$$$$$$   --------------------------    !!!!!!!!!!!\n");
  printf("              $$$$$$$$$$$      -----------     !!!!!!!!!!!\n");
  printf("                    $$$$$$$      ------      !!!!!!!\n");
 
}
