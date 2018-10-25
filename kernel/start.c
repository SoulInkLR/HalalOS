#include "debugger.h"
#include "cpu.h"
#include "ecran.h"
#include "horloge.h"
#include "test.h"
#include "processus.h"
#include <stdio.h>
#include "file_mess.h"
#include "clavier.h"


extern file_mess tab_file_mess[NBQUEUE];

int fact(int n)
{
	if (n < 2)
		return 1;

	return n * fact(n-1);
}


void kernel_start(void)
{

  int i;
  test_start();

  sti();
  i = 10;
  
  i = fact(i);
  
  while(1)
   hlt();
  
  return;

}
