 #include "kbd.h"

int aff_ON = 1;
int fin_buff = -1; //pointe sur le dernier caractere valable dans le buffer
char buffer[TAILLE_BUFFER];


/*fonction qui reçoit le texte converti à
partir des scancodes sous la forme de string C */

void keyboard_data(char *str)
{
  /*
  //premiere version : test
  printf("Codes ASCII de la touche : ");
  for (int i = 0; str[i] != '\0'; i++) {
  printf("%u ", (unsigned)str[i]);
  }
  if ((str[0] >= 32) && (str[0] < 127)) {
  printf(", symbole imprimable : '%c' ", str[0]);
  }
  printf("\n");
  
  */  

  //deuxième version
  //caractères imprimables
  
  if((str[0] >= 32 ) && (str[0] < 127 )){
    if(aff_ON == 1)
      printf("%c", str[0]);
    fin_buff ++;   
    buffer[fin_buff] =  str[0];
    //  printf("entree buff= %c \n", buffer[fin_buff]);
  }

  //touche entree
  if(str[0] == 13){
    if(aff_ON == 1)
      printf("\n");
    fin_buff++;
    buffer [fin_buff] = 13;
    fin_buff = -1;
    
    //ici on debloque le premier processus en file d'attente sur fid_clav
    psend(fid_clav[old_display_pid], 0);
  }
  //13=>entree, 9=> tab, 27=>echap
  if((str[0] < 32) && (str[0] != 13 ) && (str[0] != 9 ) && (str[0]!=27) ){

    if(aff_ON == 1)
      printf("^%c", str[0]+64);
    buffer[fin_buff] =  str[0];
  }
  if(str[0] == 9 ){
    if(aff_ON == 1)
      printf("\t");
    fin_buff ++;   
    buffer[fin_buff] =  str[0];
  }

  //touche del /!\ attention a la couleur du curseur
  if(str[0] == 127){
    
    if(aff_ON == 1 && fin_buff != -1){
      printf("\b");
      fin_buff --;
    }
  }
  /*
  // tout le reste est ignore
  if(str[0] == 8 ){
  if(aff_ON == 1)
  printf("\b");
  }

  //touche Ctrl-J
  if(str[0] == 10){
  if(aff_ON == 1)
  printf("\n");
  }
  */
 
  // affiche_buff();


}

void affiche_buff(){
  printf("\n buffer : ");
  int i = 0;
  while (i <= fin_buff){
    printf("%c",buffer[i]);
    i ++;
  }
  printf("\n");
}


//à utiliser où?
void kbd_leds(unsigned char leds) {
  outb(0xED, 0x60);
  outb(leds, 0x60);
}
