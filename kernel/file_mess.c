#include "file_mess.h"

//fonctions intermediaires 
/*fonction d'initialisation du tableau des files*/
void init_tab_file_mess(void){
  int i=0;
  while(i<NBQUEUE){
     tab_file_mess[i].fid = -1;
     i++;
  }
}

void enfiler_msg (int mess, int fid){
  message* msg = mem_alloc (sizeof(message));

  msg->val = mess;
  if (tab_file_mess[fid].msg_tete == NULL){
    msg->suiv = NULL;
    tab_file_mess[fid].msg_tete = msg;
    tab_file_mess[fid].msg_queue = msg;
  }
  else{
    (tab_file_mess[fid].msg_queue)-> suiv = msg;
    tab_file_mess[fid].msg_queue = msg;
  }
}

int defiler_msg (int fid){
  int ret = tab_file_mess[fid].msg_tete->val;
  message* temp = tab_file_mess[fid].msg_tete;
  tab_file_mess[fid].msg_tete = (tab_file_mess[fid].msg_tete)->suiv;
 
  mem_free (temp, sizeof(message));
  return ret;
}

/*------------------------------------------------------------------*/
//fonctions principales

/*fonction qui crée une file de messages
  elle renvoie le fid de la file si elle a pu etre créee, 
  -1 sinon*/
int pcreate(int count){
  if(count <= 0 || count > 100) return -1; //valeur de count non valable
  int i=0;
  while((tab_file_mess[i].fid !=-1) && i<NBQUEUE ){
    i++;
  }
  
  if (i == NBQUEUE){
    printf("files de message pleine \n");
    return -1;
  }
  
  //si on peut allouer un nouvelle file
  INIT_LIST_HEAD(&tab_file_mess[i].head_fp);
  INIT_LIST_HEAD(&tab_file_mess[i].head_fv);
  tab_file_mess[i].msg_tete = NULL;
  tab_file_mess[i].msg_queue = NULL;
  tab_file_mess[i].capacite = count;
  tab_file_mess[i].fid = i;
  tab_file_mess[i].nb_msg = 0;

  return i;
}


/*fonction qui depose un message dans la file fid,
  elle renvoie 0 si fid est valide, -1 sinon*/

int psend(int fid, int message){

  if(fid<0 || fid>=NBQUEUE){
    printf("erreur fid non reconnu \n");
    return -1;
  }
  else if (tab_file_mess[fid].fid == -1) {
    printf("erreur file de fid %d non reconnu", fid);
    return -1;
  }

  //cas 1 : file vide et processus en attente (bloqué sur file vide)
  if(tab_file_mess[fid].msg_tete == NULL && !queue_empty(&tab_file_mess[fid].head_fv) ){
    PS* ps_choisi = queue_out (&tab_file_mess[fid].head_fv, PS, proc_fv);
    ps_choisi->msg = message;

    ps_choisi->stat = activable;
    queue_add (ps_choisi, &proc_activables, PS, link_proc_activables, priorite);

    ps_choisi->has_received_msg = 1;
    /* tab_file_mess[fid].nb_msg++; */
    /* enfiler_msg (message, fid); */


    /* if (ps_choisi->priorite < tab_ps[cour_pid].priorite) { */
    /*   int old_prio = ps_choisi->priorite; */
    /*   chprio(ps_choisi->pid, _MAX_PRIO_); */
      
    /*   tab_ps[cour_pid].stat = activable; */
    /*   queue_add (&tab_ps[cour_pid], &proc_activables, PS, link_proc_activables, priorite); */

    /*   priochanged = 1; */

    /*   ordonnance(); */

    /*   chprio(ps_choisi->pid, old_prio); */
    /*   ps_choisi->stat = activable; */
    /*   queue_add (ps_choisi, &proc_activables, PS, link_proc_activables, priorite); */
    /* } */
    /* else  */
      ordonnance();
  }
  //cas 2 : file pleine, on bloque le processus sur file pleine 
  else if(tab_file_mess[fid].nb_msg == tab_file_mess[fid].capacite){
    tab_ps[cour_pid].stat = bloque_sur_file_pleine;
    tab_ps[cour_pid].fid_bloque = fid;
    tab_ps[cour_pid].msg = message;
    
    queue_add (&tab_ps[cour_pid], &tab_file_mess[fid].head_fp, PS, proc_fp, priorite);
    
    ordonnance();
    
    // Si entre temps on a eu un preset ou pdelete ou quoi
    if (tab_ps[cour_pid].fid_bloque == -1) 
      return -1;
    else 
      return 0;
  }
  //cas 3 : file ni vide ni pleine !
  else {
    tab_file_mess[fid].nb_msg++;
    enfiler_msg (message, fid);
  }


  return 0;
}


int preceive(int fid, int *message){
  if(fid<0 || fid>=NBQUEUE){
    printf("erreur fid non reconnu \n");
    return -1;
  }
  else if (tab_file_mess[fid].fid == -1) {
    printf("erreur file de fid %d non reconnu", fid);
    return -1;
  }
  
  PS *proc;
  
  //cas 1 : file pleine
  if(tab_file_mess[fid].nb_msg == tab_file_mess[fid].capacite){
    if (message != NULL)
      *message = defiler_msg (fid);
    else 
      defiler_msg(fid);
    //processus en attente
    if(!queue_empty(&tab_file_mess[fid].head_fp)){
      proc = queue_out (&tab_file_mess[fid].head_fp, PS, proc_fp);     
      //on la complete avec le message du premier proc bloque
      enfiler_msg (proc->msg, fid);
      // on passe ce processus a activable ou actif
      proc->stat = activable;
      queue_add (proc, &proc_activables, PS, link_proc_activables, priorite);
 
      if(proc->priorite > tab_ps[cour_pid].priorite)
	ordonnance();
      
      return 0;
    }
    //pas de processus en attente
    else{
      tab_file_mess[fid].nb_msg--;
      return 0;
    }
  }
  //cas 2 : file vide
  else if(tab_file_mess[fid].msg_tete == NULL){
    tab_ps[cour_pid].stat=bloque_sur_file_vide;
    tab_ps[cour_pid].fid_bloque = fid;
    queue_add (&tab_ps[cour_pid], &tab_file_mess[fid].head_fv, PS, proc_fv, priorite);
    
    ordonnance();
    
    // On est revenu après ordonnance donc un message a été ajouté à la file
    // et le processus qui n'est plus bloqué doit le récupérer
    if ((tab_ps[cour_pid].has_received_msg != 0) && (tab_ps[cour_pid].fid_bloque != -1)) {
      if (message != NULL) 
	*message = tab_ps[cour_pid].msg;
      
      tab_ps[cour_pid].has_received_msg = 0;
      tab_ps[cour_pid].msg = 0;

      ordonnance();
      
      return 0;
    }
    else return -1;
    // Si on a débloqué le processus bloqué sur file vide, mais qu'on n'a 
    //pas eu de message entre temps ajouté à la file, on return -1 car "erreur"
    // return -1;
  }
  //cas 3 : file ni vide ni pleine
  else {
    if (message != NULL)
      *message = defiler_msg (fid);
    else 
      defiler_msg(fid);
    tab_file_mess[fid].nb_msg--;
    return 0;
  }
}

/*fonction qui détruit une file de messages
  elle renvoie 0 si fid est valide, -1 sinon*/
int pdelete(int fid){
  if(fid<0 || fid>=NBQUEUE){
    return -1;
  }
  else if (tab_file_mess[fid].fid == -1) {
    printf("erreur file de fid %d non reconnu", fid);
    return -1;
  }

  PS* element;
  
  while(!queue_empty(&tab_file_mess[fid].head_fp)){
    element =  queue_out (&tab_file_mess[fid].head_fp, PS, proc_fp);
    element->stat = activable;
    queue_add (element, &proc_activables, PS, link_proc_activables, priorite); 
    // On met le champ du processus activable à -1 pour que lorsqu'il retournera dans
    // son code, il sache que sa file a été bloquée
    element->fid_bloque = -1;    }
  while(!queue_empty(&tab_file_mess[fid].head_fv)){
    element = queue_out (&tab_file_mess[fid].head_fv, PS, proc_fv);
    element->stat = activable;
    queue_add (element, &proc_activables, PS, link_proc_activables, priorite);
    // On met le champ du processus activable à -1 pour que lorsqu'il retournera dans
    // son code, il sache que sa file a été bloquée
    element->fid_bloque = -1;      
  }

  while (tab_file_mess[fid].msg_tete != NULL)
    defiler_msg (fid);
  
  tab_file_mess[fid].capacite = 0;
  tab_file_mess[fid].nb_msg = 0;
  tab_file_mess[fid].fid = -1;
  ordonnance();

  return 0;
}


/*fonction qui vide la file identifiée par fid
  initialise tout!
  return -1 si fid non valide*/
int preset(int fid){
  if(fid<0 || fid>=NBQUEUE){
    printf("erreur fid non reconnu \n");
    return -1;
  }
  else if (tab_file_mess[fid].fid == -1) {
    printf("erreur file de fid %d non reconnu", fid);
    return -1;
  }
  
  PS* element;

  while(!queue_empty(&tab_file_mess[fid].head_fp)){
    element =  queue_out (&tab_file_mess[fid].head_fp, PS, proc_fp);
    element->stat = activable;
    queue_add (element, &proc_activables, PS, link_proc_activables, priorite);
    // On met le champ du processus activable à -1 pour que lorsqu'il retournera dans
    // son code, il sache que sa file a été bloquée
    element->fid_bloque = -1;
  }
  while(!queue_empty(&tab_file_mess[fid].head_fv)){
    element = queue_out (&tab_file_mess[fid].head_fv, PS, proc_fv);
    element->stat = activable;
    queue_add (element, &proc_activables, PS, link_proc_activables, priorite);  
    // On met le champ du processus activable à -1 pour que lorsqu'il retournera dans
    // son code, il sache que sa file a été bloquée
    element->fid_bloque = -1;    
  }
  while (tab_file_mess[fid].msg_tete != NULL)
    defiler_msg (fid);
  
  tab_file_mess[fid].nb_msg = 0;

  ordonnance();

  return 0;
}

int pcount(int fid, int *count){
  if(fid<0 || fid>=NBQUEUE){
    printf("erreur fid non reconnu \n");
    return -1;
  }
  else if (tab_file_mess[fid].fid == -1) {
    printf("erreur file de fid %d non reconnu", fid);
    return -1;
  }
  
  int nb_ps_fp = 0;
  int nb_ps_fv = 0;
  PS* temp;

  if(count != NULL){
    //file vide
  
    if (queue_empty (&tab_file_mess[fid].head_fv)){
      if (queue_empty (&tab_file_mess[fid].head_fp))
	*count=tab_file_mess[fid].nb_msg;
      else{
	queue_for_each(temp, &tab_file_mess[fid].head_fp, PS, proc_fp)
	  nb_ps_fp++;
	*count= tab_file_mess[fid].nb_msg+nb_ps_fp;
      }
    }else{
      queue_for_each(temp, &tab_file_mess[fid].head_fv,PS, proc_fv)
	nb_ps_fv++;
      *count = -nb_ps_fv;
    }
    return 0;
  }
  else
    return -1;
}


