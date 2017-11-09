#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw( void ){
  int sw = ((PORTD & 0x00000f00) >> 8);
  return sw;
}

int getbtns(void){
  int btns = ((PORTD & 0x000000E0) >> 5);
  return btns;
}
