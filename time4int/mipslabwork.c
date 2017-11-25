/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-10-08 by Amy Chen & Beichen Chen

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdio.h>
#include <stdlib.h>

int mytime = 0x5957;
int timeoutcount = 0;
int prime = 1234567;

char textstring[] = "text, more text, and even more text!";
char received;

/* Interrupt Service Routine */
void user_isr( void ) {
  if(IFS(0) & (1 << 8)){
    if(timeoutcount == 9){
      time2string( textstring, mytime);
      display_string( 3, textstring);
      display_update();
      tick( &mytime);
      IFSCLR(0) = 0x100;
      timeoutcount = 0;
    } else{
      IFSCLR(0) = 0x100;
      timeoutcount = timeoutcount + 1 ;
    }
  }
  if(IFS(0) & (1 << 19)){
    PORTE = PORTE + 1;
    IFSCLR(0) = 0x80000;
  }
}

void inituart(void) {
    U1STA = 0;
    U1BRG = 520; // 42
    U1MODESET = 0x8000; // 16e bit

    U1STASET = 0x1400; // 11e bit
    //U1STASET = 0x1000; // 13e bit
}

void putcharserial(char c) {
    while (U1STA & (1 << 9)) { // 10e bit
    }
    U1TXREG = c;
}

void putstrserial(char *str) {
    int i = 0;
    while (str[i] != 0) {
        putcharserial(str[i++]);
    }
    putcharserial('\r');
    putcharserial('\n');
}

void initspi(void) {
    char junk;
    SPI1CON = 0;
    SPI1CONCLR = 0x8000; // ON = bit 15 (16e bit)
    SPI1CONCLR = 0x800; //32 bits data = bit 11 (12e bit)
    SPI1CONSET = 0x400; //16 bits data = bit 10 (11e bit)
    //SPI2CONSET = 0x1000; //SDO2 off (read only) bit 12
    junk = SPI1BUF;
    SPI1BRG = 906; // 44100Hz
    SPI1CONSET = 0x20; // MSTEN = bit 5 (6e bit)
    SPI1CONSET = 0x100; // CKE = bit 8 (9e bit)
    SPI1STATCLR = 0x40; //Clear SPIROV bit, bit 6
    // SPI2CONSET = 0x80; //SSEN bit 7
    SPI1CONSET = 0x8000; // ON = bit 15 (16e bit)

    // Configure RB2 to digital
    //AD1PCFGSET = 0x4; //redan klart i main

    // Configure RB2 to output (Connected to SS2)
    TRISBCLR = 0x4;
    PORTBSET = 0x4;
    ODCB = 0x0;
}

/* Lab-specific initialization goes here */
void labinit( void)
{
    volatile int *trise = (volatile int*) 0xbf886100;
    *trise = *trise & 0xffffff00;
    PORTESET = 0xffffff00;
    TRISDSET = 0xFE0;
    T2CONSET = (0 << 15); //Slå av timer
    TMR2 = 0;
    /*bits 4-6 bestämmer prescale, 1:64 har värdet 110, 1:256 har värdet 111.
    Med prescale 1:64 blir perioden för stor (17 bitar, dvs 1 bit för mycket) */
    T2CON = (7 << 4);
    //Period of 100 ms
    PR2 = (80000000/256)/10;
    IECSET(0) = (1 << 8); //enable the timer 2 interrupt (bit 8)
    IPCSET(2) = 0xC; //Pirority 3 (bits 2-4)
    IECSET(0) = (1 << 19);
    IPCSET(4) = (0x3 << 10);
    // asm volatile("ei"); //enable interrupts on the microcontroller
    T2CONSET = (1 << 15);
    //Calling the function enable_interrupt in labwork.S
    enable_interrupt();
    // asm volatile("ei");
    initspi();
    //inituart();
    return;
  }

void save(int amplitude, int index, int *amplitudeList[]) {
    //*(amplitudeList+index) = amplitude;
    amplitudeList[index] = amplitude;
}


/* This function is called repetitively from the main program */
void labwork(void) {
//int labwork( int amplitudeList ) {
  prime = nextprime( prime);
  display_string( 0, itoaconv( prime));
  display_update();

  // SPI
  while(!(SPI1STAT & 0x08));
  SPI1BUF = 'S';
  PORTBCLR = 0x4; // Set SS1 to 0
  while((SPI1STAT & (1 << 11)) && !(SPI1STAT & 1)); //While SPI2 is busy and receive buffer not full
  received = SPI1BUF;
  PORTBSET = 0x4; // Set SS1 to 1

  // static int index = 0;
  // save(received, index, amplitudeList);
  // index++;

  // if (index == 11025) {
  //     // Måste vi sätta en null int efter sista int i listan?
  //     display_string(1, itoaconv(sizeof(amplitudeList)/sizeof(int)));
  //     return 0;
  // }
  // return 1;




  if (received == 0) {
      display_string(1, "ZERO");
  } else {
      display_string(1, itoaconv(received));
  }
  display_update();
  //putstrserial(itoaconv_unsigned(received));
}

// void test (void) {
//     static int i = 0;
//     display_string(2, itoaconv(i));
//     i++;
//     delay(10);
// }
