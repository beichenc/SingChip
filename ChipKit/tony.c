/* tony.c

   This file written 2017 by Beichen Chen and Amy Chen

   For copyright and licensing, see file COPYING */
   // TODO: Check copyrights, I mean, there's absolutely no code in this file not written by us only.

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "headers.h"  /* Declatations for these labs */
#include <stdio.h> // TODO: are we using this?
#include <stdlib.h> // TODO: are we using this?
#include "fft.h"

const short fft_sample_rate = 4000;
const short fft_size = 1024;
// // TODO: Verify this value
const short backgroundSound = 100;
const short hanning_factor = 32768;

// Change global values?
int toneIndex = 0;
char toneList[10][9];
short started = 0;
short number_of_saved_songs = 3;

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
    init_songLibrary();
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
int getValidTone(short frequency, char* tone) {
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

void saveFrequencyAsTone(char* tone) {
    strcpy(toneList[toneIndex],tone);
    display_string(1, tone);
    display_update();
}

void stop(void) {
    started = 0;
    // Stop knapp -> start comparing toneList to our database. Get back an int that corresponds to a song.
    int songIndex = identify();
    display_string(3, itoaconv(songIndex));
    if (songIndex == -1) {
        display_string(2, "Much wrong");
    } else {
        char songName[MAX_NAME_LENGTH];
        strcpy(songName, songLibrary[songIndex][0]);
        display_string(2, songName);
    }
    display_update();
    toneIndex = 0;
    int i;
    for (i = 0; i < MAX_SONG_LENGTH; i++) {
        strcpy(toneList[i],"0");
    }
    //delay(5000);
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
        amplitudeList[i] = hanning[i] * amplitudeList[i] / hanning_factor;
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

    static char tone[9];
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
    int validTone = getValidTone(frequency, tone);
    if (validTone == 1) {
        saveFrequencyAsTone(tone);
        toneIndex++;
    }
    // Tone sequence full - we ignore all values and wait for stop button to be pressed
    if (toneIndex == MAX_SONG_LENGTH) {
        // TODO: Delay or just exit as if stop button pressed
        display_string(3, "U played 2 much! LOL");
        display_update();
        stop();
        return;
    }
}

// Found online
int string2int(char a[]) {
  int c, sign, offset, n;

  if (a[0] == '-') {  // Handle negative integers
    sign = -1;
  }

  if (sign == -1) {  // Set starting position to convert
    offset = 1;
  }
  else {
    offset = 0;
  }

  n = 0;

  for (c = offset; a[c] != '\0'; c++) {
    n = n * 10 + a[c] - '0';
  }

  if (sign == -1) {
    n = -n;
  }

  return n;
}

// Found online
int compare_strings(char a[], char b[])
{
   int c = 0;

   while (a[c] == b[c]) {
      if (a[c] == '\0' || b[c] == '\0')
         break;
      c++;
   }

   if (a[c] == '\0' && b[c] == '\0')
      return 0;
   else
      return -1;
}

int identify() {
    int i, j;
    short tlength = toneIndex;
    // TODO:
    for (i = 0; i < number_of_saved_songs; i++) {
        short slength = string2int(songLibrary[i][1]); // Length saved in index 1
        if (tlength == slength) { // Each song in the library contains two extra elements
            for (j = 0; j < tlength; j++) {
                display_update();
                if (!(compare_strings(toneList[j],songLibrary[i][j+2]) == 0)) {
                    break;
                }

                // När man godkänt sista tonen
                if (j == tlength-1) {
                    display_string(1, "Wow");
                    display_update();
                    return i;
                }
            }
        }
    }
    // No song matched
    return -1;
}

// char *strcpy(char *dest, const char *src)
// {
//    char *save = dest;
//    while(*dest++ = *src++);
//    return save;
// }

char *strcpy(char *dest, const char *src)
{
  unsigned i;
  for (i=0; src[i] != '\0'; ++i)
    dest[i] = src[i];

  return dest;
}



/* This function is called repetitively from the main program */
//void labwork(void) {
int tony( void ) {
    if (getbtns() & (1 << 2)) {
        started = 1;
    }

    if(getbtns() & (1 << 1)){
        stop();
    }
  return 1;
}

/* Interrupt Service Routine */
void user_isr( void) {
    static short amplitudeList[1024];
    static char received;
    static short sample_counter;

    if(IFS(0) & 0x100){
        if (started == 1) {
            // SPI
            while(!(SPI1STAT & 0x08));
            SPI1BUF = 'S';
            PORTBCLR = 0x4; // Set SS1 to 0
            while((SPI1STAT & (1 << 11)) && !(SPI1STAT & 1)); // While SPI2 is busy and receive buffer not full
            received = SPI1BUF;
            PORTBSET = 0x4; // Set SS1 to 1
          if(sample_counter == fft_size){
              // Sample array full - Time to do FFT
            do_fft(amplitudeList);
            IFSCLR(0) = 0x100;
            sample_counter = 0;
          } else{
              // Save to sample array
              // TODO: What was the benefit of using static instead of global?
              save(received, sample_counter, amplitudeList);
            IFSCLR(0) = 0x100;
            sample_counter = sample_counter + 1;
          }
      } else {
          IFSCLR(0) = 0x100;
      }
  }
}
