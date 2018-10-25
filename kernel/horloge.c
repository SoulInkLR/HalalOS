#include "horloge.h"


//traitement d'interruption appellé par traitement.S
//dans un premier temps son role est d'afficher l'horloge
//plustard il servira aussi à appeller la fonction ordonnance()
void traitant_horloge (){
  
  outb(0x20, 0x20);
  irq++;

  compteur++;
  if(compteur == 50){
    compteur=0;
    s++;
    if(s == 60){
      s=0;
      m++;
    }
    if (m == 60){
      m = 0;
      h = (h+1)%24;
    }
    
  }
  char buffer[10];
  sprintf(buffer,"<%02d:%02d:%02d>", h, m, s);
  //on écrit l'heure dans un buffeur
  for(int k = 0; k < 10;k++){
    //ecriture par caractère d'ou la boucle
    //à chaque fois on fait un update de l'heure
    short *ptr = (short*)0xB808A+k;
    *ptr = (short)(2<<8|buffer[k]);
  }
  //appel à la fonction ordonnance() des processus
  
  ordonnance();
  
}

//initialisation du traitant d'interuption : 
//on met l'adresse du traitant dans la case 
//correspondante a l'interruption horloge

void init_traitant (int num_IT, void (*traitant)(void)){
  unsigned long *adr;
  adr = (unsigned long*)((int)0x1000+(int)num_IT*8);
  *adr=KERNEL_CS<<16 | (((unsigned long)(traitant)) & 0xFFFF);
  if (num_IT == 49)
    *(adr+1) = 0xEE00 | (((unsigned long)(traitant)) & 0xFFFF0000);
  else
    *(adr+1) = 0x8E00 | (((unsigned long)(traitant)) & 0xFFFF0000);

}


//initilisation des compteurs
void init_h(){
  s = 0;
  m = 0;
  h = 0;
  compteur = 0;
  irq = 0;
}

// initialisation de la fréquence à 50Hz
void frequence_horloge (void){
  outb (0x34,0x43);
  outb ((QUARTZ/CLOCKFREQ) % 256, 0x40);
  outb (((QUARTZ/CLOCKFREQ)&0xFF00)>>8 , 0x40);
}


void clock_settings(unsigned long *quartz, unsigned long *ticks){
  *quartz = QUARTZ;
  *ticks = CLOCKFREQ;
 
}
  
unsigned long current_clock(){
  return irq;
}

//masquage/démasquage de l'irq
void masque_IRQ_slave (int num_IRQ, int masque){
   unsigned char m;
    
   m = inb(0xA1);
 
  if(masque == 1)
    outb (m | (masque<<(num_IRQ)), 0x21);
   
  else
    outb ( m & (0xFF - (1<<num_IRQ)), 0x21);
      
}

  
//masquage/démasquage de l'irq
void masque_IRQ (int num_IRQ, int masque){
   unsigned char m;
    
   m = inb(0x21);
 
  if(masque == 1)
    outb (m | (masque<<(num_IRQ)), 0x21);
   
  else
    outb ( m & (0xFF - (1<<num_IRQ)), 0x21);
      
}



void wait_clock(unsigned long clock){

  tab_ps[cour_pid].stat = endormi;
  unsigned long i = clock+current_clock();
  tab_ps[cour_pid].duree = i;
  sti();
  hlt();
  cli();
}
