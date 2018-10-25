#include "allocateur.h"
#include "cpu.h"


unsigned long page_user[49152];

void mem_init(void){ 
  
  unsigned long temp = 0x4000000;
  for(int i=0; i<49152; i++){
    memset ((void*)(temp+i*0x1000), 0, 4096);
    page_user[i] = temp+i*0x1000;
  }
}

void* alloc_user_page (void){

  unsigned long ret;
  for (int i = 0; i<49152; i++){
    if (page_user [i]) {
      ret = page_user [i];
      page_user [i] = 0;
      return (void*)ret;
    }
 
  }
  return NULL;
}
void free_user_page (void* ptr){

 
  if ((((unsigned long)ptr) & 0xFFF) != 0) assert (0); 
  unsigned long i = (((unsigned long)ptr)-0x4000000)/0x1000;
 
  memset (ptr, 0, 4096);
  page_user [i] = (unsigned long)ptr;
 
}

//fonction pour initialiser une zone memoire
//avec des 0
void init_zone (void* ptr,unsigned long size){
  memset (ptr, 0, size);
}


