/* tony.c

   This file written 2017 by Beichen Chen and Amy Chen

   For copyright and licensing, see file COPYING */
   // TODO: Check copyrights, I mean, there's absolutely no code in this file not written by us only.

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "headers.h"  /* Declatations for these labs */
#include <stdio.h>
#include <stdlib.h>
#include "fft.h"

const short fft_sample_rate = 4000;
const short fft_size = 1024;
const short song_library_size = 10;
const short max_name_length = 10;
const short max_song_length = 10;
// // TODO: Verify this value
const short backgroundSound = 50;
char toneList[10][16];

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
    // Configure RB2 to output (Connected to SS1)
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
    // TODO: We don't need the following interrupt?
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

// A valid tone is one that follows an empty (background sound) or different tone, and is not background sound
int getValidTone(short frequency, char* tone, int toneIndex) {
    static int newTone = 1;
    // We ignore background sound, but this is an indicator that it's time to listen for a new tone.
    if (frequency < backgroundSound) {
        newTone = 1;
        return 0;
    }

    // Not background sound

    // If toneIndex is 0, save, so we don't get index out of bounds error in next if.
    if (toneIndex == 0) {
        newTone = 0;
        return 1;
    }

    // If it's a new tone (e.g. one separated from previous by empty sound), it's valid.
    if (newTone == 1) {
        newTone = 0;
        return 1;
    }

    // Same tone as previously without any empty tone in between
    return 0;
}

void saveFrequencyAsTone(char* tone, int toneIndex) {
    strcpy(toneList[toneIndex],tone);
}

void do_fft(short* amplitudeList) {
    static short imaginaryList[1024];
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

    static char tone[4];
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

    // Check validity of frequency and save if valid
    static int toneIndex = 0;
    int validTone = getValidTone(frequency, tone, toneIndex);
    if (validTone) {
        saveFrequencyAsTone(tone, toneIndex);
    }
    // Tone sequence full - we ignore all values and wait for stop button to be pressed
    if (toneIndex == 10) {
        display_string(3, "You played too much! LOL");
        return;
    } else {
        toneIndex++;
    }
}

// TODO: Make sure this works for the shorter array aswell
short length(char (*toneList)[max_song_length+2]) {
    short i = 0;
    // End of array marker
    while (toneList[i][0] != '0' && sizeof(toneList[i]) == 1) {
        i++;
    }
    return i;
}

int identify() {
    int i, j;
    short tlength = length(toneList);
    for (i = 0; i < 2; i++) {
        if (tlength == length(songLibrary[i])-2) { // Each song in the library contains two extra elements
            for (j = 0; j < tlength; j++) {
                if (toneList[j] != songLibrary[i][j+1]) {
                    break;
                }

                // När man godkänt sista tonen
                if (j == tlength-1) {
                    return i;
                }
            }
        }
    }
    // No song matched
    return -1;
}

char *strcpy(char *dest, const char *src)
{
   char *save = dest;
   while(*dest++ = *src++);
   return save;
}

/* This function is called repetitively from the main program */
//void labwork(void) {
int tony( void ) {

  return 1;
}

/* Interrupt Service Routine */
void user_isr( void) {
    static short amplitudeList[1024];
    static char received;
    static short timeoutcount;

    if(IFS(0) & 0x100){
        //Button 2
        static short started = 0;

        // Start playing
        if((getbtns() & 1) && started == 0){
          started = 1;
        }

        // Stop playing and identify song
        if ((getbtns() & 1) && started == 1) {
            started = 0;
            // Stop knapp -> start comparing toneList to our database. Get back an int that corresponds to a song.
            int songIndex = identify();
            if (songIndex == -1) {
                display_string(2, "Not found");
            } else {
                short max_name_length = 10;
                char songName[max_name_length];
                strcpy(songName, songLibrary[songIndex][0]);
                display_string(2, songName);
            }
            int i;
            for (i = 0; i < max_song_length; i++) {
                strcpy(toneList[i],"0");
            }
        }

        if (started == 1) {
            // SPI
            while(!(SPI1STAT & 0x08));
            SPI1BUF = 'S';
            PORTBCLR = 0x4; // Set SS1 to 0
            while((SPI1STAT & (1 << 11)) && !(SPI1STAT & 1)); // While SPI2 is busy and receive buffer not full
            received = SPI1BUF;
            PORTBSET = 0x4; // Set SS1 to 1
          if(timeoutcount == fft_size){
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
      } else {
          IFSCLR(0) = 0x100;
      }
  }
}
