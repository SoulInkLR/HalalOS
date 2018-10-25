#include "fct_kernel.h"


int select_call (int num, int arg1, int arg2, int arg3, int arg4){

  switch (num){

  case 1:
    return start((const char*)arg1, (unsigned long) arg2, (int)arg3, (void*)arg4);
  case 2:
    return getpid ();
    
  case 3:
    return getprio (arg1);
    
  case 4:
    return chprio (arg1, arg2);
    
  case 5:
    return kill (arg1);

  case 6:
    return waitpid (arg1, (int*)arg2);

  case 7:
    exit (arg1);
    return 0;

  case 8:
    return current_clock ();

  case 9:
    clock_settings ((unsigned long*)arg1, (unsigned long*)arg2);
    return 0;

  case 10:
    wait_clock ((unsigned long)arg1);
    return 0;

  case 11:
    return pcreate (arg1);
    
  case 12:
    return pdelete (arg1);

  case 13:
    return psend (arg1, arg2);
    
  case 14:
    return preceive (arg1, (int*)arg2);

  case 15:
    return pcount (arg1, (int*)arg2);

  case 16:
    return preset (arg1);
    
  case 17:
    cons_write ((const char *)arg1, (long) arg2);
    return 17;
    
  case 18:
    cons_echo (arg1);
    return 18;
 
  case 19:
    return cons_read ((char*) arg1, (unsigned long) arg2);

  case 20:
    return (int)shm_create ((const char*)arg1);
    
  case 21:
    return (int)shm_acquire ((const char*)arg1);
    
  case 22:
    shm_release ((const char*) arg1);
    return 22; 
  
  case 23:
    efface_ecran();
    return 23;

  case 24:
    funct_sinfo((int) arg1);
    return 24;

  case 25:
    funct_ps((int) arg1);
    return 25;

  
  case 26:
    display_banner();
    return 26;
  
  case 27:
    switch_display((int) arg1);
    return 27;


  }
  return -1;

}
