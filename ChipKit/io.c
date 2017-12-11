/* io.c

   This file written 2017 by Beichen Chen and Amy Chen

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include <pic32mx.h>
#include "headers.h"

int getsw( void ){
  int sw = ((PORTD & 0x00000f00) >> 8);
  return sw;
}

int getbtns(void){
  int btns = ((PORTD & 0x000000E0) >> 5);
  return btns;
}
