/* tony.c

   This file written 2017 by Beichen Chen and Amy Chen

   For copyright and licensing, see file COPYING */
   // TODO: Check copyrights, I mean, there's absolutely no code in this file not written by us only.

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "headers.h"  /* Declatations for these labs */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fft.h"

// TODO: Refactor global variables
int timeoutcount = 0;
int fft_sample_rate = 4000;
const int fft_size = 1024;
short amplitudeList[1024];
short imaginaryList[1024];
// // TODO: Verify this value
// short backgroundSound = 50;
// char toneList[7][];
char tone[3];

char textstring[] = "text, more text, and even more text!";
char received;

// Testing
int number_of_ffts_done = 0;

// TODO: Use volatile values?

void initspi(void) {
    char junk;
    SPI1CON = 0;
    SPI1CONCLR = 0x8000; // ON = bit 15 (16e bit)
    SPI1CONCLR = 0x800; // 32 bits data = bit 11 (12e bit)
    SPI1CONSET = 0x400; // 16 bits data = bit 10 (11e bit)
    junk = SPI1BUF;
    SPI1BRG = 8; // 4.7Mhz, större SPI1BRG fungerar inte - varför?
    // SPI1BRG = 7 // 5.3 Mhz
    SPI1CONSET = 0x20; // MSTEN = bit 5 (6e bit)
    SPI1CONSET = 0x100; // CKE = bit 8 (9e bit)
    SPI1STATCLR = 0x40; // Clear SPIROV bit, bit 6
    SPI1CONSET = 0x8000; // ON = bit 15 (16e bit)
    // Configure RB2 to output (Connected to SS2)
    TRISBCLR = 0x4;
    PORTBSET = 0x4;
    ODCB = 0x0;
}

void initchip( void)
{
    volatile int *trise = (volatile int*) 0xbf886100;
    *trise = *trise & 0xffffff00;
    PORTESET = 0xffffff00;
    TRISDSET = 0xFE0;
    T2CONSET = (0 << 15); //Slå av timer
    TMR2 = 0;
    T2CONCLR = (7 << 4);
    PR2 = 20000; // 4000Hz
    IECSET(0) = (1 << 8); //enable the timer 2 interrupt (bit 8)
    IPCSET(2) = 0xC; //Pirority 3 (bits 2-4)
    IECSET(0) = (1 << 19);
    IPCSET(4) = (0x3 << 10);
    T2CONSET = (1 << 15);
    //Calling the function enable_interrupt in labwork.S
    enable_interrupt();
    initspi();
    return;
}

  int maximum(short* array) {
      int i;
      int max = 0;
      int maxindex;
      // Kollar upp till size, då vi redan har halverat listan innan vi skickat in,
      //då får vi vår max frekvens (Nyqvist frekvens).
      for (i = 0; i < fft_size/2; i++) {
          if (array[i] > max) {
              max = array[i];
              maxindex = i;
          }
      }
      return maxindex;
  }

  // Function found online and modified to operate on arrays.
  void squareroot(short* realList, short* imaginaryList, short* fftOutput){
    int i;
    short num;
    short res;
    short bit; // The second-to-top bit is set: 1 << 30 for 32 bits
    for(i=0;i<fft_size/2;i++){
      res = 0;
      bit = 1 << 14;
      num = (realList[i] * realList[i] + imaginaryList[i] * imaginaryList[i]);
      // "bit" starts at the highest power of four <= the argument.
      while (bit > num)
          bit >>= 2;

      while (bit != 0) {
          if (num >= res + bit) {
              num -= res + bit;
              res = (res >> 1) + bit;
          }
          else
              res >>= 1;
          bit >>= 2;
      }
      fftOutput[i] = res;
    }
  }

void save(short amplitude, int index, short* amplitudeList) {
    *(amplitudeList+index) = amplitude;
}

// // A valid tone is one that follows an empty (background sound) or different tone, and is not background sound
// int validTone(short frequency, char* tone, int toneIndex) {
//     static int newTone = 1;
//     // We ignore background sound, but this is an indicator that it's time to listen for a new tone.
//     if (frequency < backgroundSound) {
//         newTone = 1;
//         return 0;
//     }
//
//     // Not background sound
//
//     // If toneIndex is 0, save, so we don't get index out of bounds error in next if.
//     if (toneIndex == 0) {
//         newTone = 0;
//         return 1;
//     }
//
//     // If it's a new tone (e.g. one separated from previous by empty sound), it's valid.
//     if (newTone == 1) {
//         newTone = 0;
//         return 1;
//     }
//
//     // Same tone as previously without any empty tone in between
//     return 0;
// }
//
// void saveFrequencyAsTone(char* tone, int toneIndex) {
//     toneList[toneIndex] = tone;
// }

void do_fft(short* amplitudeList) {
    // TODO: Behöver vi initialisera alla värden till 0?
    static int imindex;
    imindex = 0;
    while(imindex < fft_size){
        imaginaryList[imindex] = 0;
        imindex++;
    }

    //Applicera Hanning window på amplitudelist
    int i;
    for (i = 0; i < fft_size; i++) {
        amplitudeList[i] = hanning[i] * amplitudeList[i];
    }

    // Använd bibliotek
    gst_spectrum_fix_fft(amplitudeList, imaginaryList, 10, 0);

    //Pytagoras sats på den nya amplitudeList och imaginaryList
    short fftOutput[fft_size/2];
    squareroot(amplitudeList, imaginaryList, fftOutput);

    // Ta max av amplitudeList, ta ut index och räkna ut frekvens.
    int indexOfMax = maximum(fftOutput);
    int frequency = indexOfMax*fft_sample_rate/fft_size;
    // char freqstr[6] = "Freq: ";
    // char freqline[9];
    // int a;
    // for (a = 0; a < 9; a++) {
    //     if (a < 6) {
    //         freqline[a] = freqstr[a];
    //     } else {
    //         freqline[a] = itoaconv(frequency)[a-6];
    //     }
    // }
    display_string(3, itoaconv(frequency));

    freqToTone(frequency, tone);
    // char tonestr[6] = "Tone: ";
    // char toneline[9];
    // int j;
    // for (j = 0; j < 9; j++) {
    //     if (j < 6) {
    //         toneline[j] = tonestr[j];
    //     } else {
    //         toneline[j] = tone[j-6];
    //     }
    // }
    display_string(2, tone);
    display_update();

    // // Check validity of frequency and save if valid
    // static int toneIndex = 0;
    // int validTone = validTone(tone, toneIndex);
    // if (validTone) {
    //     saveFrequencyAsTone(tone, toneIndex);
    // }
    // // Tone sequence full - we ignore all values and wait for stop button to be pressed
    // if (toneIndex == 7) {
    //     return;
    // } else {
    //     toneIndex++;
    // }
}

// int identify(char* toneList) {
//     // TODO: Change later to match max size
//     int size = 7;
//     int i;
//     // TODO: make constant for songLibrary size - what is best practice?
//     for (i = 0; i < 2; i++) {
//         for (j = 0; j < size; j++) {
//             if (toneList[j] != songLibrary[i][j+1]) {
//                 break;
//             }
//
//             // När man godkänt sista tonen
//             // TODO: kolla att det inte finns flera toner efter detta, dvs. skilt från 0/C0.
//             if (j == size-1) {
//                 return i;
//             }
//         }
//     }
//
//     return -1;
// }

char *strcpy(char *dest, const char *src)
{
   char *save = dest;
   while(*dest++ = *src++);
   return save;
}

/* This function is called repetitively from the main program */
//void labwork(void) {
int tony( void ) {
    // IF START knapp tryckt -> Enable interrupts?
    // while (stop knapp != 1)

  // Stop knapp == 1 -> Disable interrupts? start comparing toneList to our database. Get back an int that corresponds to a song.
  // int songIndex = identify(toneList);
  // // TODO: set max characters for name. now 10.
  // char *songName[10];
  // strcpy(songName, songLibrary[songIndex][0]);
  // display_string(2, songName);
  // -1 -> Not found
  // If -1 -> display not found. otherwise display song name.
  return 1;
}

/* Interrupt Service Routine */
void user_isr( void) {
    if(IFS(0) & 0x100){
        // SPI
        while(!(SPI1STAT & 0x08));
        SPI1BUF = 'S';
        PORTBCLR = 0x4; // Set SS1 to 0
        while((SPI1STAT & (1 << 11)) && !(SPI1STAT & 1)); // While SPI2 is busy and receive buffer not full
        received = SPI1BUF;
        PORTBSET = 0x4; // Set SS1 to 1
      if(timeoutcount == fft_size){
          number_of_ffts_done++;
          // Sample array full - Time to do FFT
        do_fft(amplitudeList);
        IFSCLR(0) = 0x100;
        timeoutcount = 0;
      } else{
          // Save to sample array
          // TODO: What was the benefit of using static instead of global?
          save(received, timeoutcount, amplitudeList);
        IFSCLR(0) = 0x100;
        timeoutcount = timeoutcount + 1;
      }
  }
}
