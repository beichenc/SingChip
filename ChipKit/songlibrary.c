/* songlibrary.c

   This file written 2017 by Beichen Chen and Amy Chen

   For copyright and licensing, see file COPYING */
   // TODO: Check copyrights, I mean, there's absolutely no code in this file not written by us only.

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "headers.h"

char blinka[MAX_SONG_LENGTH+2][MAX_NAME_LENGTH] = {"BLINKA","7","C4","C4","G4","G4","A4","A4","G4"};
char rudolf[MAX_SONG_LENGTH+2][MAX_NAME_LENGTH] = {"RUDOLF","7","G4", "A4", "G4", "E4", "C5", "A4", "G4"};
char elise[MAX_SONG_LENGTH+2][MAX_NAME_LENGTH] = {"ELISE","9","E5","D#5","E5","D#5","E5","B4","D5","C5","A4"};
char songLibrary[SONG_LIBRARY_SIZE][MAX_SONG_LENGTH+2][MAX_NAME_LENGTH];
char letters[26] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

void strarraycpy(char (*dest)[MAX_NAME_LENGTH], char (*src)[MAX_NAME_LENGTH]) {
    int i;
    for (i = 0; i < MAX_SONG_LENGTH+2; i++) {
        strcpy(dest[i],src[i]);
    }
}

void init_songLibrary(void) {
    int j;
    strarraycpy(songLibrary[0],blinka);
    strarraycpy(songLibrary[1],rudolf);
    strarraycpy(songLibrary[2],elise);
}

char getLetter(short letterIndex) {
    return letters[letterIndex];
}
