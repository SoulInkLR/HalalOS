#include "mapping.h"


void map(unsigned long* cr3, unsigned long* adr_v, unsigned long* adr_ph, unsigned long permission){
  unsigned long index_pd =  (unsigned long) adr_v>>22;
  unsigned long index_pt = (unsigned long) adr_v >>12 & 0x003FF;
  unsigned long* case_tb = NULL; 
  
  if(*(cr3+index_pd) == 0){
    *(cr3+index_pd) = (unsigned long)alloc_user_page ();
  
    memset ((void*)*(cr3+index_pd),0,4096);
  }
  
  case_tb = (unsigned long*)(*(cr3+index_pd) & ~0xFFF);
  
  *(case_tb + index_pt) = ((unsigned long)adr_ph & ~0xFFF) + (permission & 0xFFF);
  
    if (permission)
      *(cr3 + index_pd) = (*(cr3+index_pd) & ~0xFFF) + (permission & 0xFFF);
}


// fonction qui initialise la zone memoire pointe par
// cr3 avec des zero et copy les 64 entrees du cr3 kernel

void init_cr3_idle (void* ptr){
 
  init_zone (ptr, 4096);
  unsigned long* cr3 = (unsigned long*)0x2001C;
  memcpy (ptr, (void*)*cr3, 4096);
 
}

void init_cr3 (void* ptr_cr3){
  init_zone (ptr_cr3, 4096);
  memcpy (ptr_cr3, (void*)tab_ps[0].save[5], 256);
  
}
