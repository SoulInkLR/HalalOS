#include "mouse.h"

void init_pics (int offset1, int offset2){

  //unsigned char a1, a2;
 
        inb(PIC1_DATA);                        // save masks
	inb(PIC2_DATA);
 
	outb(ICW1_INIT+ICW1_ICW4, PIC1_COMMAND);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(ICW1_INIT+ICW1_ICW4, PIC2_COMMAND);
	io_wait();
	outb(offset1, PIC1_DATA);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(offset2, PIC2_DATA);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(4, PIC1_DATA);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(2, PIC2_DATA);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(ICW4_8086, PIC1_DATA);
	io_wait();
	outb(ICW4_8086, PIC2_DATA);
	io_wait();
	
}

void init_mouse (void){
 
  unsigned char status_byte;
  cour_x = 0;
  cour_y = 0;
  //enable the auxiliary mouse device
  mouse_wait (1);
  outb(0xA8, 0x64);

  //modify the "compaq status byte", to enable the interrupt.
  mouse_wait(1);
  outb(0x20, 0x64);
  //receive the status byte at port 0x60
  mouse_wait(0);
  status_byte = (inb(0x60) | 2);
  //send back
  //a.tell the controller that we will be sending a status byte
  mouse_wait(1);
  outb(0x60, 0x64);
  //b. send the modified status_byte
  mouse_wait(1);
  outb(status_byte, 0x60);

  //Settings : default
  mouse_write(0xF6);
  mouse_read();

  //enable the mouse
  mouse_write(0xF4);
  mouse_read();

  //Set up irq 12 handler
  init_traitant (44, handler_mouse);

}

//wait for the mouse to be able to send or receive data
//or for the timeout to pass
void mouse_wait (int w_r){
  
  
  unsigned int _time_out=100000;
  if(w_r==0)
  {
    while(_time_out--) //Data
    {
      if((inb(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((inb(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

//This function is designed to simplify the process 
//of sending commands to the mouse

void mouse_write(unsigned char a_write){
  //Wait to be able to send a command
  mouse_wait(1);
  //Tell the mouse we are sending a command
  outb(0xD4, 0x64);
  //Wait for the final part
  mouse_wait(1);
  //Finally write
  outb(a_write, 0x60);

}


//waits to be able to read data from the mouse,
// and then returns it to the caller
unsigned char mouse_read (void){

  //Get response from mouse
  mouse_wait(0);
  return inb(0x60);

}

void place_mouse(int lig, int col){
  char buffer[1];
  sprintf(buffer,"<");
  //on écrit l'heure dans un buffeur
     //ecriture par caractère d'ou la boucle
    //à chaque fois on fait un update de l'heure
  short *ptr = ptr_mem(lig,col);
  *ptr = (short)(2<<8|buffer[0]);
 
}
	

void traitant_mouse(void)
{
  //aquitement du traitant
  outb(0x20, 0xA0); 
  outb(0x20, 0x20);

  static unsigned char cycle = 0;
  static char mouse_bytes[3];
  mouse_bytes[cycle++] = inb(0x60);
  int next_x = 0;
  int next_y = 0;
  if (cycle == 3) { // if we have all the 3 bytes...
    cycle = 0; // reset the counter

    // do what you wish with the bytes, this is just a sample
    if ((mouse_bytes[0] & 0x80) || (mouse_bytes[0] & 0x40))
      return; // the mouse only sends information about overflowing,
              //do not care about it and return
    next_y = cour_y - (int)mouse_bytes [1];
    next_x = cour_x + (int)mouse_bytes [2];
    printf("mouse a bougge \n");
    place_mouse (next_y, next_x);
    /*if (!(mouse_bytes[0] & 0x20))
      y |= 0xFFFFFF00; //delta-y is a negative value
    if (!(mouse_bytes[0] & 0x10))
      x |= 0xFFFFFF00; //delta-x is a negative value
    */
    /*
    if (mouse_bytes[0] & 0x4)
      printf("Middle button is pressed!n");
    if (mouse_bytes[0] & 0x2)
      printf("Right button is pressed!n");
    if (mouse_bytes[0] & 0x1)
      printf("Left button is pressed!n");
    */
  }
}


