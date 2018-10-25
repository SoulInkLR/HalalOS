#include "clavier.h"


/*
Interruption associée aux événements clavier : IRQ1
vecteur 33 , défini par OSMon au niveau du PIC

Ports
Lecture des scancodes depuis le port 0x60
Configuration du clavier : port 0x64

Registres d’écritures
Registre de Commandes : port 0x64
Registre de Données : port 0x60

Registres de lecture
Registre d'état : port 0x64
Registre de données : port 0x60

*/


void init_buff(void){
  int i=0;
  while (i<TAILLE_BUFFER){
    buffer[i] = 0;
    i++;
  }
}

void init_clav(){
  for (int i = 0 ; i < _NB_SHELLS_ ; i++) {
    fid_clav[i] = pcreate(1);
  }

}

void traitant_clavier(){

  outb(0x20, 0x20);

  unsigned char m;
  m = inb(0x60);

  do_scancode(m);
}


void cons_echo(int on){
  aff_ON = on;
}

int cons_write(const char *str, long size){
  console_putbytes(str, size);
  return 0;
}

/* fonction qui retourne les premiers length caracteres si le buffer
   n'est pas vide, sinon il met le processus dans une file d'attente*/
unsigned long cons_read(char *string, unsigned long length){
  if (length == 0) {
    return 0;
  }
  
  //si le buffer est vide 
  //on met le processus dans la file d'attente
  if(fin_buff == -1){
    preceive(fid_clav[tab_ps[cour_pid].display], &mess);
    //le processus est mis dans la file d'attente sur file vide de le file de messages fid_clav
  }
 
  //si le buffer n'est pas vide
  unsigned long ret, i=0;
  
    //on recopie ce qu'il faut dans string
  while(i < length){
    // si le caractere n'est pas entree
    if(buffer[i] != 13) {
      string[i] = buffer[i];

      i++;
    }
    //sinon on sort de la boucle
    else
      break;
  }
    
  ret = i;
  
  //si le dernier caractere n'est pas entree (on a copié la taille maxi)
  if(buffer[i] != 13){
   int j=0;
    //tant que le suivant n'est pas entree et qu'on est dans la taille du buffer
    //while((buffer[i] != 13) && (i<(unsigned long)TAILLE_BUFFER)){
    while(i<(unsigned long)TAILLE_BUFFER){
        
      buffer[j] = buffer[i];
      i++;
      j++;
    }
  }
  //si le caractere est entree et que la taille est maxi
  else if((buffer[i] == 13) && (ret==(unsigned long)TAILLE_BUFFER)){
    //printf("prochain recupere ligne vide\n");
    //le prochain proc recupere une ligne vide
    buffer[0] = 13;
  }else if((buffer[i] == 13)  && (ret<(unsigned long)TAILLE_BUFFER)){

    init_buff();
  }
 
  return ret;
}

