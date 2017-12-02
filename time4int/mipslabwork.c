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
#include "kiss_fft.h"

int mytime = 0x5957;
int timeoutcount = 0;
int prime = 1234567;
int fft_sample_rate = 8000;
const int fft_size = 2048;
short amplitudeList[2048];
// char twinkleSong[7];
// // TODO: Verify this value
// short backgroundSound = 50;
// char toneList[7][];

char textstring[] = "text, more text, and even more text!";
char received;

// Testing
int number_of_ffts_done = 0;

void initspi(void) {
    char junk;
    SPI1CON = 0;
    SPI1CONCLR = 0x8000; // ON = bit 15 (16e bit)
    SPI1CONCLR = 0x800; //32 bits data = bit 11 (12e bit)
    SPI1CONSET = 0x400; //16 bits data = bit 10 (11e bit)
    //SPI2CONSET = 0x1000; //SDO2 off (read only) bit 12
    junk = SPI1BUF;
    //SPI1BRG = 249; // 10000Hz (sets of 16-bit data per second)
    SPI1BRG = 8; // 4.7Mhz, större SPI1BRG fungerar inte - varför?
    // SPI1BRG = 7 // 5.3 Mhz
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
    // /*bits 4-6 bestämmer prescale, 1:64 har värdet 110, 1:256 har värdet 111.
    // Med prescale 1:64 blir perioden för stor (17 bitar, dvs 1 bit för mycket) */
    //T2CON = (2 << 4); //1:256
    //PR2 = 31250;
    T2CONCLR = (7 << 4);
    //Period of 0.000125 ms (8000Hz)
    PR2 = 10000;
    IECSET(0) = (1 << 8); //enable the timer 2 interrupt (bit 8)
    IPCSET(2) = 0xC; //Pirority 3 (bits 2-4)
    IECSET(0) = (1 << 19);
    IPCSET(4) = (0x3 << 10);
    // // asm volatile("ei"); //enable interrupts on the microcontroller
    T2CONSET = (1 << 15);
    //Calling the function enable_interrupt in labwork.S
    enable_interrupt();
    // asm volatile("ei");
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
              // display_string(3, itoaconv(max));
              // display_update();
              //delay(100);
              maxindex = i;
          }
      }
      return maxindex;
  }

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

  // void squareroot(short* realList, short* imaginaryList, short* fftOutput){
  //   // Now we find the square root of realNumbers[k] using a very
  //           // fast (but compiler dependent) integer approximation:
  //           // (adapted from: http://www.codecodex.com/wiki/Calculate_an_integer_square_root)
  //     long place;
	// 		long root;
  //     int k;
  //     for(k = 0; k < fft_size/2; k++){
  //       place = 0x40000000;
  // 			root = 0;
  //       fftOutput[k] = (realList[k] * realList[k] + imaginaryList[k] * imaginaryList[k]);
  //       if (fftOutput[k] >= 0) // Ensure we don't have a negative number
  // 			{
  // 				while (place > fftOutput[k]) place = place >> 2;
  //
  // 				while (place)
  // 				{
  // 					if (fftOutput[k] >= root + place)
  // 					{
  // 						fftOutput[k] -= root + place;
  // 						root += place * 2;
  // 					}
  // 					root = root >> 1;
  // 					place = place >> 2;
  // 				}
  // 			}
  // 			fftOutput[k] = root;
  //     }
  // }

void save(short amplitude, int index, short* amplitudeList) {
    *(amplitudeList+index) = amplitude;
    short amplitude2 = *(amplitudeList+index);
    if(amplitude==amplitude2){
      // display_string(1, itoaconv(amplitude));
      // display_update();
    }
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
    short imaginaryList[fft_size];
    static int imindex = 0;
    while(imindex < fft_size){
      imaginaryList[imindex] = 0;
      imindex++;
    }
    // Använd bibliotek
    //fix_fft(amplitudeList, imaginaryList, 10);
    //gst_spectrum_fix_fft(amplitudeList, imaginaryList, 10, 0);
    kiss_fft_cfg cfg = kiss_fft_alloc( fft_size ,0 ,0,0 );

        while ...

            ... // put kth sample in cx_in[k].r and cx_in[k].i

            kiss_fft( cfg , cx_in , cx_out );

            ... // transformed. DC is in cx_out[0].r and cx_out[0].i

        free(cfg);

    //Pytagoras sats på den nya amplitudeList och imaginaryList
    short fftOutput[fft_size/2];
    squareroot(amplitudeList, imaginaryList, fftOutput);

    // short fftOutput[2];
    // fftOutput[0] = 0;
    // fftOutput[1] = 0;
    // short list1[4];
    // list1[0] = 1;
    // list1[1] = 3;
    // list1[2] = 1;
    // list1[3] = 1;
    // short list2[4];
    // list2[0] = 1;
    // list2[1] = 4;
    // list2[2] = 1;
    // list2[3] = 1;
    // squareroot(list1, list2, fftOutput);
    // Ta max av amplitudeList, ta ut index och räkna ut frekvens.
    int indexOfMax = maximum(fftOutput);
    int frequency = indexOfMax*fft_sample_rate/fft_size;
    display_string(3, itoaconv(frequency));
    display_update();

    // // char tone[] = freqToTone(frequency);
    // //
    // // // Check validity of frequency and save if valid
    // // static int toneIndex = 0;
    // // int validTone = validTone(tone, toneIndex);
    // // if (validTone) {
    // //     saveFrequencyAsTone(tone, toneIndex);
    // // }
    // // // Tone sequence full - we ignore all values and wait for stop button to be pressed
    // // if (toneIndex == 7) {
    // //     return;
    // // } else {
    // //     toneIndex++;
    // // }


    // int i;
    // int max = 0;
    // int maxindex;
    // // Kollar upp till size, då vi redan har halverat listan innan vi skickat in,
    // //då får vi vår max frekvens (Nyqvist frekvens).
    // for (i = 0; i < fft_size/2; i++) {
    //     //display_string(2, itoaconv(i));
    //     //display_update();
    //     if (fftOutput[i] > max) {
    //         max = fftOutput[i];
    //         maxindex = i;
    //     }
    // }




    //i=0;
    //display_string(3, itoaconv(maxindex));

    // //Check if fftOutput is empty
    // int i;
    // int zerolist = 1;
    // for(i = 0; i< sizeof(fftOutput)/sizeof(short); i++){
    //   if(fftOutput[i]!=0){
    //     zerolist = 0;
    //     display_string(2,itoaconv(fftOutput[i]));
    //   }
    // }
    // if(zerolist==1){
    //   display_string(2, "ZERO LIST");
    // }
}

// int identify(char* toneList) {
//     // TODO: Change later to match max size
//     int size = 7;
//     int i;
//     for (i = 0; i < size; i++) {
//         if (toneList[i] != twinkleSong[i]) {
//             return -1;
//         }
//     }
//     // TODO: Change to return the right song later.
//     return 1;
// }

/* This function is called repetitively from the main program */
//void labwork(void) {
int labwork( void ) {
    // IF START knapp tryckt -> while (stop knapp != 1)
  //listen();

  // Stop knapp == 1 -> start comparing toneList to our database. Get back an int that corresponds to a song.
  // -1 -> Not found
  // If -1 -> display not found. otherwise display song name.
  return 1;
}

/* Interrupt Service Routine */
// void user_isr( void ) {
//   if(IFS(0) & (1 << 8)){
//       // SPI
//       while(!(SPI1STAT & 0x08));
//       SPI1BUF = 'S';
//       PORTBCLR = 0x4; // Set SS1 to 0
//       while((SPI1STAT & (1 << 11)) && !(SPI1STAT & 1)); //While SPI2 is busy and receive buffer not full
//       received = SPI1BUF;
//       PORTBSET = 0x4; // Set SS1 to 1
//
//     if(timeoutcount == 2048){
//         number_of_ffts_done++;
//         display_string(2, "Full");
//         display_string(0, itoaconv(number_of_ffts_done));
//         display_update();
//         // Sample array full - Time to do FFT
//       do_fft(amplitudeList);
//       display_string(3, "FFT done");
//       display_update();
//       IFSCLR(0) = 0x100;
//       timeoutcount = 0;
//
//     } else{
//         display_string(2, "NOT full");
//         display_update();
//         // Save to sample array
//         // TODO: What was the benefit of using static instead of global?
//         save(received, timeoutcount, amplitudeList);
//
//       IFSCLR(0) = 0x100;
//       timeoutcount = timeoutcount + 1 ;
//     }
//   }
//   if(IFS(0) & (1 << 19)){
//     PORTE = PORTE + 1;
//     IFSCLR(0) = 0x80000;
//   }
// }

// int listen( void){
//   if(IFS(0) & 0x100){
//       // SPI
//       // while(!(SPI1STAT & 0x08));
//       // SPI1BUF = 'S';
//       // PORTBCLR = 0x4; // Set SS1 to 0
//       // while((SPI1STAT & (1 << 11)) && !(SPI1STAT & 1)); //While SPI2 is busy and receive buffer not full
//       // received = SPI1BUF;
//       // PORTBSET = 0x4; // Set SS1 to 1
//       received = 42; // Testing, but this gives same speed as with SPI so nothing wrong with SPI baud rate
//     if(timeoutcount == 2048){
//         number_of_ffts_done++;
//         display_string(2, "Full");
//         display_string(0, itoaconv(number_of_ffts_done));
//         display_update();
//         // Sample array full - Time to do FFT
//       //do_fft(amplitudeList);
//       display_string(3, "FFT done");
//       display_update();
//       IFSCLR(0) = 0x100;
//       timeoutcount = 0;
//       return 1;
//     } else{
//         display_string(2, "NOT full");
//         display_string(3, itoaconv(timeoutcount));
//         display_update();
//         // Save to sample array
//         // TODO: What was the benefit of using static instead of global?
//         save(received, timeoutcount, amplitudeList);
//       IFSCLR(0) = 0x100;
//       timeoutcount = timeoutcount + 1 ;
//       return 0;
//     }
//   } else{
//     return 0;
//   }
// }

/* Interrupt Service Routine */
void user_isr( void) {
    if(IFS(0) & 0x100){
        // SPI
        while(!(SPI1STAT & 0x08));
        SPI1BUF = 'S';
        PORTBCLR = 0x4; // Set SS1 to 0
        while((SPI1STAT & (1 << 11)) && !(SPI1STAT & 1)); //While SPI2 is busy and receive buffer not full
        received = SPI1BUF;
        PORTBSET = 0x4; // Set SS1 to 1
        //received = 42; // Testing, but this gives same speed as with SPI so nothing wrong with SPI baud rate
      if(timeoutcount == fft_size){
          number_of_ffts_done++;
          display_string(0, itoaconv(number_of_ffts_done));
          display_update();
          // Sample array full - Time to do FFT
        do_fft(amplitudeList);
        IFSCLR(0) = 0x100;
        timeoutcount = 0;
      } else{
          // display_string(2, "NOT full");
          // display_string(3, itoaconv(timeoutcount));
          // display_update();
          // Save to sample array
          // TODO: What was the benefit of using static instead of global?
          save(received, timeoutcount, amplitudeList);
        IFSCLR(0) = 0x100;
        timeoutcount = timeoutcount + 1;
      }
  }
}
