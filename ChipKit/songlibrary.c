/* songlibrary.c

   This file written 2017 by Beichen Chen and Amy Chen

   For copyright and licensing, see file COPYING */
   // TODO: Check copyrights, I mean, there's absolutely no code in this file not written by us only.

const char const blinka[12][16] = {"Blinka","C4","C4","G4","G4","A4","A4","G4","0"};
const char const rudolf[12][16] = {"Rudolf","G4", "A4", "G4", "E4", "C5", "A4", "G4","0"};
char songLibrary[10][12][16];

// TODO: MUST REWRITE!
void init_songLibrary(void) {
    int j;
    short max_song_length = 10;
    short max_name_length = 16;
    short song_library_size = 2; // Current size
    for (j = 0; j < max_song_length+2; j++) {
        strcpy(songLibrary[0][j],blinka[j]);
    }
    int k;
    for (k = 0; k < max_song_length+2; k++) {
        strcpy(songLibrary[1][k],rudolf[k]);
    }
}
