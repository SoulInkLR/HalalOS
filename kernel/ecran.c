#include "ecran.h"
#include "queue.h"
#include "mem.h"

link link_prec_char;
LIST_HEAD(link_prec_char); 

struct display_switcher display_sw[_NB_SHELLS_];
int old_display_pid = 0;
int prio_char_cour = 0;


//restauration de l'affichage pour basculer d'un shell à l'autre
void switch_display (int target_pid) {
  // On sauvegarde le display actuel
  memmove(display_sw[old_display_pid].old_display, (short*)ptr_mem(0,0), 4000);
  // On restaure à l'écran le nouveau contexte à charger
  memmove((short*)ptr_mem(0,0), display_sw[target_pid].old_display, 4000);
  place_curseur(display_sw[target_pid].ligne, display_sw[target_pid].colonne);
  
  old_display_pid = target_pid;
  tab_ps[0].display = old_display_pid;
}

//calcul l'adresse du pointeur en mémoire à partir de 
//l'emplacement dans l'ecran (ligne et colonne)
short *ptr_mem(int lig, int col){
  
  short *adr;
  int x;
  x = 2*(lig*80+col);

  if (tab_ps[cour_pid].display != old_display_pid && cour_pid != 0) {
    adr = (short*)(display_sw[tab_ps[cour_pid].display].old_display) + x/2;
  }
  else
    adr = (short*)(0xB8000+x);
  return(adr);
}

//ecrit le caractère c à l'endroit (ligne,colonne) de l'ecran
//avec les options : couleur, fond et clignotement

void ecrit_car(int lig,int col,unsigned short coul,unsigned short fond,
	       unsigned short clig, char c){
  short *adr;
  short val=0x0000;
  adr=(short*)ptr_mem(lig,col);
  val=val+(((clig<<7) | (fond<<4) | (coul))<<8);
  val=val+c;
  *adr=val;
}


//place le curseur sur l'ecran à l'emplacement (ligne,colonne)

void place_curseur(int lig,int col){
  if (tab_ps[cour_pid].display == old_display_pid || cour_pid == 0) {
    int pos_cur=lig*80+col;
    unsigned char pfaible;
    unsigned char pfort;
    pfaible= (unsigned char)(pos_cur&0xFF);
    pfort=(unsigned char)((pos_cur>>8)&0xFF);
    outb(0x0F,0x3D4);
    outb(pfaible,0x3D5);
    outb(0x0E,0x3D4);
    outb(pfort,0x3D5);
  }  
}


//efface tout l'ecran et reinitialise ligne et colonne à 0

void efface_ecran(void){
 
  int i,j;
  for(i=0;i<25;i++){
    for(j=0;j<80;j++){
      ecrit_car(i,j,15,0,0,32);
    }
  }

  
  display_sw[tab_ps[cour_pid].display].ligne=0;
  display_sw[tab_ps[cour_pid].display].colonne=0;
  place_curseur(0,0);

}

void insert_prec_char (char c) {
  if (c == 8)
    return;
  prec_char* prec = mem_alloc(sizeof(prec_char));
  prec->c = c;
  prec->prio = prio_char_cour++;
  queue_add(prec, &link_prec_char, prec_char, l_prec_char, prio);
}

prec_char* retrieve_prec_char() {
  if (!queue_empty(&link_prec_char))
    return queue_out(&link_prec_char, prec_char, l_prec_char);
  else 
    return NULL;
}


//si il y a un caractère particulier, cette fonction fait le traitement
//necessaire, sinon elle appelle la fonction ecrit_car 

void traite_car(char c){
  
  prec_char* old_prec = NULL;

  if(( c>=0 && c<=31)||(c==127)) {
    
    switch (c){
      
      // \b backspace
    case 8:
      old_prec = retrieve_prec_char();
      if (old_prec != NULL && old_prec->c == 9) 
	display_sw[tab_ps[cour_pid].display].colonne = display_sw[tab_ps[cour_pid].display].colonne - 8;
      else 
	display_sw[tab_ps[cour_pid].display].colonne--;
      ecrit_car(display_sw[tab_ps[cour_pid].display].ligne, display_sw[tab_ps[cour_pid].display].colonne, 2, 0, 0, ' ');
      place_curseur(display_sw[tab_ps[cour_pid].display].ligne, display_sw[tab_ps[cour_pid].display].colonne);
      
      break;
      
      // \t tab
    case 9:
      display_sw[tab_ps[cour_pid].display].colonne = display_sw[tab_ps[cour_pid].display].colonne+8;
      place_curseur(display_sw[tab_ps[cour_pid].display].ligne,display_sw[tab_ps[cour_pid].display].colonne);
      
      break;
      
      // \n newline
    case 10:
      display_sw[tab_ps[cour_pid].display].colonne = 0;
      display_sw[tab_ps[cour_pid].display].ligne++;
      place_curseur(display_sw[tab_ps[cour_pid].display].ligne,display_sw[tab_ps[cour_pid].display].colonne);
      
      break;

      // \f
    case 12:
      efface_ecran();
      break;
      
      // \r
    case 13:
      display_sw[tab_ps[cour_pid].display].colonne=0;
      place_curseur(display_sw[tab_ps[cour_pid].display].ligne,display_sw[tab_ps[cour_pid].display].colonne);
      
      break;
    }
  }
  else{
    ecrit_car(display_sw[tab_ps[cour_pid].display].ligne,display_sw[tab_ps[cour_pid].display].colonne,2,0,0,c);
    display_sw[tab_ps[cour_pid].display].colonne ++;
  }

  insert_prec_char(c);
  
}

 
   
// fonction qui gère le defilement de l'ecran

void defilement(void){
 
  short *src=(short*)ptr_mem(1, 0);
  short *dst=(short*)ptr_mem(0, 0);
  memmove(dst, src,4000);
  for(int i = 0; i<80;i++)
    ecrit_car(24,i,0,0,0,' ');
  display_sw[tab_ps[cour_pid].display].ligne--;
 
}


//fonction appellée par printf et utilise toutes les autres fonctions pour 
// faire le traitement demmandé => un printf correcte

void console_putbytes(const char *chaine, int taille){
  
  int i=0;
place_curseur(display_sw[tab_ps[cour_pid].display].ligne,display_sw[tab_ps[cour_pid].display].colonne);
    
  while (i<taille){
       
    if (display_sw[tab_ps[cour_pid].display].colonne==79 && chaine[i] != 8){
      traite_car(chaine[i]);
      display_sw[tab_ps[cour_pid].display].colonne = 0;
      display_sw[tab_ps[cour_pid].display].ligne++;
    }
    else if (display_sw[tab_ps[cour_pid].display].ligne > 24){ 
      defilement();
      traite_car(chaine[i]);
           
    }
    else 
      traite_car(chaine[i]);
         
    place_curseur(display_sw[tab_ps[cour_pid].display].ligne,display_sw[tab_ps[cour_pid].display].colonne);
    i++;
  }
}

