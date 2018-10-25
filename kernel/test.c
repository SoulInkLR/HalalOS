#include "test.h"


//initialisation du tableau de ps
int init_tab_ps(void){
  cour_pid = 0;
  for(int i = 0; i < NBPROC; i++)
    tab_ps[i].stat = vide;
  tab_ps[0].save[5] = (unsigned long) alloc_user_page ();
  init_cr3_idle ((unsigned long*)tab_ps[0].save[5]);
  tab_ps[0].stat = actif; 
  tab_ps[0].priorite = 0;
  tab_ps[0].display = old_display_pid;
  
  INIT_LIST_HEAD (&(tab_ps[0].link_fils));
  return 0;
}

void idle(void){
  //  display_banner();
  // beep();
  start("shell", 4096, 128, NULL);
  start("shell", 4096, 128, NULL);
  start("shell", 4096, 128, NULL);
  start("shell", 4096, 128, NULL);

  for(;;){
    
    sti();
    hlt();
    cli();
  }


}


void test_start(void)
{

  efface_ecran();
  //initialisation de la souris
  init_pics (0x20, 0x28);
  init_mouse ();
  masque_IRQ_slave (12, 0);
  masque_IRQ (2, 0);
  //initialisation du clavier
  init_traitant (33, handler_clavier);
  masque_IRQ (1, 0);
  //initialisation des files de messages pour le clavier
  init_tab_file_mess();
  init_clav();
  //initialisation de l'horloge
  init_h();
  init_traitant (32, handler_horloge);
  masque_IRQ (0, 0);
  frequence_horloge ();
  //initialisation de l'int49
  init_traitant (49, handler_syscall);
  //initialisation de la mémoire, table des processus, et tables des symboles
  mem_init();
  init_tab_ps();
  init_tab_apps ();
  // gestion des page faults : 
  masque_IRQ(14,0);
  init_traitant(14, handler_page_fault);
 
  init_virt();

  idle();
  hlt();
  sti();  
  
     
  while(1)
    hlt();
  
  return;


}

