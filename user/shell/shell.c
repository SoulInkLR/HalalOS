#include "shell.h"


int main(){
  
  while (1){
    printf("halalOS-shell:%d>", getpid()); 
    trait_cmd();
  }
}

void trait_cmd (void){
  char read [_TAILLE_BUFF_];
  unsigned long taille_cmd = 0;
  int num = 0;
  int arg = 0;
  
  taille_cmd = cons_read (read, _TAILLE_BUFF_);
  read[taille_cmd] = '\0';
  parsecmd (read, taille_cmd, &num, &arg);
  exec (num, arg);
      
  waitpid(-1, 0);
}



void parsecmd (char* cmd,
	       unsigned long taille_cmd,
	       int* num,
	       int* arg){
  
  char *cmd_temp;
  int j = 0;

  for(unsigned long i = 0; i <= taille_cmd; i++) {
    // Lecture de l'argument
    if (j != 0 && cmd[i] == '\0'){
      
      cmd_temp = mem_alloc(i-j+1);
      strncpy(cmd_temp, cmd+j, i-j);
      cmd_temp[i-j] = '\0';

      if (*num == 4){
	if(!strcmp(cmd_temp, "on"))
	  *arg = 1;
	else if(!strcmp(cmd_temp, "off"))
	  *arg = 0;
      }
      else 
	*arg = (int) strtol((const char*)cmd_temp, NULL, 0);
	
      
	//printf("  arg: %d ", *arg);
      mem_free(cmd_temp, i-j+1);
    } 
    // On est dans le premier mot (unique ou non)
    if (j == 0 && (cmd[i] == ' ' || cmd[i] == '\0')) {
      
      cmd_temp = mem_alloc(i+1);
      strncpy(cmd_temp, cmd, i+1);
      cmd_temp[i] = '\0';
      //      printf("cmd_temp: %s ",cmd_temp);// *num);      //echo
       
      if(!strcmp(cmd_temp, "help"))
	*num = 0;
      else if(!strcmp(cmd_temp, "ps"))
	*num = 1;
      else if(!strcmp(cmd_temp, "sinfo"))
	*num = 2;
      else if(!strcmp(cmd_temp, "exit"))
	*num = 3;
      else if(!strcmp(cmd_temp, "echo"))
	*num = 4;
      else if(!strcmp(cmd_temp, "kill"))
	*num = 5;
      else if(!strcmp(cmd_temp, "clear"))
	*num = 6;
      else if(!strcmp(cmd_temp, "banner"))
	*num = 7;
      else if(!strcmp(cmd_temp, "test"))
	*num = 8;
      else if(!strcmp(cmd_temp, "shell"))
	*num = 9;
      else if(!strcmp(cmd_temp, ""))
	*num = 10;
      else
	*num = 20;
      
      mem_free(cmd_temp, i+1);
      j = i+1;
    }   
 

    if(arg != NULL){}
  } 
}

void exec (int num, int arg){
  switch (num){
   
  case 0 :  //help
    printf("Help : liste des commandes \n");
    printf("\thelp\n");
    printf("\tps\n");
    printf("\tsinfo\n");
    //printf("\texit \n");
    printf("\techo       argument : on/off\n");
    printf("\tclear\n");
    printf("\tkill       argument : (int) numProc (le pid du processus a tuer compris entre 5 et 300)\n");
    printf("\tshell      argument : (int) numShell (le numero du shell a afficher soit 1, 2, 3 ou 4)\n");
    break;
    
  case 1 : //ps
    
    printf("PS : liste des processus \n");
    printf("PID\tETAT\tNOM \n ");
    funct_ps(getpid());
    
    break;
    
  case 2 : //sinfo
    printf("Informations sur les listes d'attente \n");
    
    printf("FID\tNBMESS\tCAPACITE\tCOUNT\n");
    funct_sinfo(getpid());
    
    break;
    /*
  case 3 :  //exit
    printf("Exit du shell avec la valeur de retour %d", arg);
    exit(arg);
    break;
    */
  case 4 :  //echo
    if(arg == 1 || arg == 0){
      printf("Echo %d\n", arg);
      cons_echo(arg);
    }
    else
      printf("argument non valable \n");
    break;

  case 5 : //kill
    if (arg >= 5)
      kill(arg);
    else
      printf("Impossible de kill un processus de pid <= 4 (idle ou shells)");
    break;

  case 6 : //clear
    efface_ecran();
    break;

  case 7 : //banniere
    display_banner();
    break;

  case 8 : //test
    switch(arg){
    case 0 : 
      start("test0", 4096, 128, NULL );
      break;
    case 1 : 
      start("test1", 4096, 128, NULL );
      break;
    case 2 : 
      start("test2", 4096, 128, NULL );
      break;
    case 3 : 
      start("test3", 4096, 128, NULL );
      break;
    case 4 : 
      start("test4", 4096, 128, NULL );
      break;
    case 5 : 
      start("test5", 4096, 128, NULL );
      break;
    case 6 :
      start("test6", 4096, 128, NULL );
      break;
    case 7 : 
      start("test7", 4096, 128, NULL );
      break;
    case 8 : 
      start("test8", 4096, 128, NULL );
      break;
    case 9 : 
      start("test9", 4096, 128, NULL );
      break;
    case 10 : 
      start("test10", 4096, 128, NULL );
      break;
    case 11 : 
      start("test11", 4096, 128, NULL );
      break;
    case 12 :
      start("test12", 4096, 128, NULL );
      break;
    case 13 : 
      start("test13", 4096, 128, NULL );
      break;
    case 14 : 
      start("test14", 4096, 128, NULL );
      break;
    case 15 : 
      start("test15", 4096, 128, NULL );
      break;
    case 16 :
      start("test16", 4096, 128, NULL );
      break;
    case 17 : 
      start("test17", 4096, 128, NULL );
      break;
    case 18 : 
      start("test18", 4096, 128, NULL );
      break;
    case 19 : 
      start("test19", 4096, 128, NULL );
      break;
    case 20 : 
      start("test20", 4096, 128, NULL );
      break;
    case 21 : 
      start("autotest", 4096, 128, NULL );
      break;
    default:
      printf("ID de test non reconnu\n");
      break;
    }
    break;
    
    //chgt de shell
  case 9 :
    if (arg > 0 || arg <= 4) {
      switch_display(arg-1);
    }
    else 
      printf("Numéro de shell non reconnu\n");
    break;
    
  case 10:
    break;
    
   default :
     printf("Commande non reconnue\n");
     break;
  }

}


