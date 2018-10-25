#include "sound.h"


//Play sound using built in speaker
void play_sound(int nFrequence) {
  int Div;
  int tmp;
  
  //Set the PIT to the desired frequency
  Div = 1193180 / nFrequence;
  outb(0xb6, 0x43);
  outb((int)Div, 0x42);
  outb((int)Div >> 8, 0x42);
  
  //And play the sound using the PC speaker
  tmp = inb(0x61);
  if (tmp != (tmp | 3)) {
    outb(tmp | 3, 0x61);
  }
}

//make it shutup
void nosound(void) {
  int tmp = (inb(0x61) & 0xFC);
  
  outb(tmp, 0x61);
}

//Make a beep
void beep(void) {
  play_sound(1000);
  wait_clock(10);
  nosound();
  
}
