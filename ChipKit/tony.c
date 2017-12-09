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

int toneIndex = 0;
char toneList[MAX_SONG_LENGTH+2][MAX_NAME_LENGTH];
short started = 0;
short number_of_saved_songs = 3;

short mode = IDENTIFY_MODE; // Default is identify mode

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
    //volatile int *porte = (volatile int*) 0xbf886110;
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

void display_play(void) {
    display_string(1, " ");
    display_string(2, "Plz play");
    display_string(3, " ");
    display_update();
    display_image(96, icon);
}

void clearToneList(void) {
    int i;
    int j;
    for (i = 0; i < MAX_SONG_LENGTH+2; i++) {
        for (j = 0; j < MAX_NAME_LENGTH; j++) {
            toneList[i][j] = '\0';
        }
    }
    if (mode == IDENTIFY_MODE) {
        toneIndex = 0;
    } else if (mode == TEACHING_MODE) {
        toneIndex = 2;
    }
}

// Stops and clears toneList and toneIndex
void startOver(void) {
    started = 0;
    clearToneList();
    display_play();
}

// Tony's lightshow
void victoryLights(void) {
    int i;
    int lightsOff = 0xffffff00;
    int lightsOn = 0xffffffff;
    for (i = 0; i < 8; i++) {
        PORTESET = lightsOff + (1 << i);
        delay(100);
    }
    for (i = 7; i >= 0; i--) {
        PORTECLR = (1 << i);
        delay(100);
    }
    for (i = 0; i < 3; i++) {
        PORTESET = lightsOn;
        delay(100);
        PORTECLR = 0x000000ff;
        delay(100);
    }
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

void addLetterToName(short* taughtSongNameIndex, int offset, char* taughtSongName) {
    if (*taughtSongNameIndex >= MAX_NAME_LENGTH) {
        display_string(3, "2 much name!"); // Name length is too long
        display_update();
        display_image(96, icon);
        delay(1500);
    } else {
        if (getsw() < 13) {
            taughtSongName[*taughtSongNameIndex] = getLetter(getsw()+offset);
            *taughtSongNameIndex = *taughtSongNameIndex+1;
            display_string(3, taughtSongName);
            display_update();
        } else {
            display_string(3, "Much wrong");
            display_update();
            display_image(96, icon);
        }
    }
}

void save_to_library(void) {
    char taughtSongName[MAX_NAME_LENGTH];
    int i;
    for (i = 0; i < MAX_NAME_LENGTH; i++) {
        taughtSongName[i] = '\0';
    }
    short taughtSongNameIndex = 0;
    display_string(1, "");
    display_string(2, "Plz name");
    display_string(3, taughtSongName);
    display_update();
    display_image(96, icon);
    while(1) {
        // Button 4 - First half of alphabet
        if (getbtns() & (1 << 2)) {
            while(getbtns() & (1 << 2)); // Block so button only updates on falling edge
            addLetterToName(&taughtSongNameIndex, 0, taughtSongName);
        }

        // Button 3 - Second half of alphabet
        if(getbtns() & (1 << 1)){
            while (getbtns() & (1 << 1)); // Block so button only updates on falling edge
            addLetterToName(&taughtSongNameIndex, 13, taughtSongName);
        }

        // Button 2 - Save and clear toneList for next round
        if (getbtns() & 1) {
            while(getbtns() & 1);
            if (taughtSongNameIndex != 0) {
                strcpy(toneList[0],taughtSongName);
                strcpy(toneList[1],itoaconv(toneIndex-2));
                strarraycpy(songLibrary[number_of_saved_songs],toneList);
                number_of_saved_songs++;
                clearToneList();
                started = 0;
                display_string(3, "Very song"); // Song successfully learned
                display_update();
                display_image(96, icon);
                delay(1000);
                display_play();
                break;
            } else {
                display_string(3, "Uhoh! Much empty"); // Name is empty, so you have to write something
                display_update();
                display_image(96, icon);
            }
        }
    }
}

void stop(short tooMuch) {
    started = 0;
    if (mode == IDENTIFY_MODE) {
        // Stop button -> start comparing toneList to our database. Get back an int that corresponds to a song.
        int songIndex = identify();
        if (songIndex == -1) {
            if (tooMuch == 0) {
                display_string(1, "");
                display_string(2, "Much wrong");
                display_string(3, "");
                display_update();
                display_image(96, icon);
            }
        } else {
            display_string(1, "");
            display_string(3, "I know!!! XD");
            display_update();
            delay(2000);
            display_string(3, songLibrary[songIndex][0]);
            display_update();
            display_image(96, icon);
            victoryLights();
        }
        clearToneList();
    } else if (mode == TEACHING_MODE) {
        save_to_library();
    }
}

// Perform FFT, check if valid, convert frequency to tone and save tone
void do_fft(short* amplitudeList) {
    static short imaginaryList[1024];
    static int imindex;
    imindex = 0;
    while(imindex < fft_size){
        imaginaryList[imindex] = 0;
        imindex++;
    }

    //Apply Hanning window on amplitudelist
    int i;
    for (i = 0; i < fft_size; i++) {
        amplitudeList[i] = hanning[i] * amplitudeList[i] / hanning_factor;
    }

    // Use fft library
    gst_spectrum_fix_fft(amplitudeList, imaginaryList, 10, 0);

    //Pythagoras theorem on the new amplitudeList and imaginaryList
    short fftOutput[fft_size/2];
    squareroot(amplitudeList, imaginaryList, fftOutput);

    // Take max of amplitudeList, find the index och get the frequency.
    int indexOfMax = maximum(fftOutput);
    int frequency = indexOfMax*fft_sample_rate/fft_size;
    display_string(3, itoaconv(frequency));

    static char tone[MAX_NAME_LENGTH];
    freqToTone(frequency, tone);
    display_string(2, tone);
    display_update();

    // Check validity of frequency and save if valid
    int validTone = getValidTone(frequency, tone);
    if (validTone == 1) {
        saveFrequencyAsTone(tone);
        toneIndex++;
    }
    // Tone sequence full - we ignore all values and wait for stop button to be pressed
    if (toneIndex == MAX_SONG_LENGTH && mode == IDENTIFY_MODE) {
        display_string(1, "");
        display_string(2, "LOL!!!");
        display_string(3, "U play 2much");
        display_update();
        display_image(96, icon);
        stop(1);
        return;
    } else if (toneIndex == MAX_SONG_LENGTH+2 && mode == TEACHING_MODE) {
        display_string(1, "");
        display_string(2, "LOL!!!");
        display_string(3, "U play 2much");
        display_update();
        display_image(96, icon);
        startOver();
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
    for (i = 0; i < number_of_saved_songs; i++) {
        short slength = string2int(songLibrary[i][1]); // Length saved in index 1
        if (tlength == slength) { // Each song in the library contains two extra elements
            for (j = 0; j < tlength; j++) {
                if (!(compare_strings(toneList[j],songLibrary[i][j+2]) == 0)) {
                    break;
                }

                // När man godkänt sista tonen
                if (j == tlength-1) {
                    display_string(2, "Wow");
                    display_update();
                    display_image(96, icon);
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

void display_menu(void) {
    display_string(1, " ");
	display_string(2, "Identify: 4");
	display_string(3, "Teaching: 3");
}

void viewSong(short songIndex) {
    int i;
    for (i = 0; i < MAX_SONG_LENGTH; i++) {
        if (compare_strings(songLibrary[songIndex][i],"") == 0) {
            return;
        }
        //display_string(1, itoaconv(i));
        display_string(2, songLibrary[songIndex][i+2]);
        display_update();
        delay(800);
        display_string(2, "");
        display_update();
        delay(150);
    }
}

void viewSongsList(void) {
    short songIndex = 0;
    display_string(1, "");
    display_string(2, songLibrary[songIndex][0]);
    display_string(3, "");
    display_update();
    while(1) {
        // Button 3 - Go forward
        if (getbtns() & (1 << 1) && started == 0) {
            while(getbtns() & (1 << 1)); // Block so button only updates on falling edge
            songIndex++;
            songIndex = songIndex % number_of_saved_songs;
            display_string(2, songLibrary[songIndex][0]);
            display_update();
        }

        // Button 4 - Examine song
        if (getbtns() & (1 << 2) && started == 0) {
            while(getbtns() & (1 << 2)); // Block so button only updates on falling edge
            viewSong(songIndex);
            display_string(2, songLibrary[songIndex][0]);
            display_update();
        }

        // Button 2 - Go back to menu
        if ((getbtns() & 1) && started == 0) {
            while(getbtns() & 1); // Block so button only updates on falling edge
            display_menu();
            display_update();
            display_image(96, icon);
            break;
        }
    }
}

int menu(void) {
    // Button 4 - Identify mode
    if (getbtns() & (1 << 2) && started == 0) {
        while(getbtns() & (1 << 2)); // Block so button only updates on falling edge
        mode = IDENTIFY_MODE;
        toneIndex = 0;
        display_play();
        return 0;
    }

    // Button 3 - Teaching mode
    if(getbtns() & (1 << 1) && started == 0){
        while(getbtns() & (1 << 1)); // Block so button only updates on falling edge
        mode = TEACHING_MODE;
        toneIndex = 2; // Begin on second index to make space for name and length
        display_play();
        return 0;
    }

    if ((getbtns() & 1) && started == 0) {
        while (getbtns() & 1);
        viewSongsList();
    }
    return 1;
}

// This function is called repetitively from the main program
int tony( void ) {
    // Starting - Button 4
    if (getbtns() & (1 << 2) && started == 0) {
        while(getbtns() & (1 << 2)); // Block so button only updates on falling edge
        if ((mode == TEACHING_MODE) && (number_of_saved_songs >= SONG_LIBRARY_SIZE)) {
            display_string(2, "Much full"); // Song library is full, so you can't teach Tony anymore songs
            display_update();
            display_image(96, icon);
        } else {
            started = 1;
        }
        return 1;
    }

    // Stopping - Button 3
    if(getbtns() & (1 << 1) && started == 1){
        while(getbtns() & (1 << 1)); // Block so button only updates on falling edge
        stop(0);
        return 1;
    }

    // Exit mode, go back to menu - Button 2 when game has not started
    if((getbtns() & 1) && started == 0) {
        while(getbtns() & 1); // Block so button only updates on falling edge
        display_menu();
        display_update();
        display_image(96, icon);
        return 0;
    }

    // Cancel game and start over - Button 2 when game has started
    if ((getbtns() & 1) && started == 1) {
        while(getbtns() & 1); // Block so button only updates on falling edge
        startOver();
    }

    return 1;
}

// Interrupt Service Routine
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
              save(received, sample_counter, amplitudeList);
            IFSCLR(0) = 0x100;
            sample_counter = sample_counter + 1;
          }
      } else {
          IFSCLR(0) = 0x100;
      }
  }
}
