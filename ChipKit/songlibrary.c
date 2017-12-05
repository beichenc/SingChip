/* songlibrary.c

   This file written 2017 by Beichen Chen and Amy Chen

   For copyright and licensing, see file COPYING */
   // TODO: Check copyrights, I mean, there's absolutely no code in this file not written by us only.

const char const blinka[12][16] = {"Blinka","7","C4","C4","G4","G4","A4","A4","G4"};
const char const rudolf[12][16] = {"Rudolf","7","G4", "A4", "G4", "E4", "C5", "A4", "G4"};
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

// char blinka[10][4] = {"C4","C4","G4","G4","A4","A4","G4"};
// char rudolf[10][4] = {"G4", "A4", "G4", "E4", "C5", "A4", "G4"};
//
// struct Song {
//     char name[16];
//     char tones[10][4];
//     short length;
// };
//
// void strarraycpy(char (*dest)[4], char (*src)[4]) {
//     int i;
//     for (i = 0; i < 10; i++) {
//         strcpy(dest[i],src[i]);
//     }
// }
//
// struct Song songLibrary[10];
//
// void init_songLibrary(void) {
//     struct Song Blinka;
//     strcpy(Blinka.name, "Blinka");
//     strarraycpy(Blinka.tones, blinka);
//     Blinka.length = 7;
//
//     struct Song Rudolf;
//     strcpy(Rudolf.name, "Rudolf");
//     strarraycpy(Rudolf.tones, rudolf);
//     Rudolf.length = 7;
//
//     songLibrary[0] = Blinka;
//     songLibrary[1] = Rudolf;
// }

// void init_songLibrary(void) {
//     struct Song Blinka = {
//         "Blinka",
//         {"C4","C4","G4","G4","A4","A4","G4"},
//         7
//     };
//
//     struct Song Rudolf = {
//         "Rudolf",
//         {"G4", "A4", "G4", "E4", "C5", "A4", "G4"},
//         7
//     };
//
//     // songLibrary[0] = Blinka;
//     // songLibrary[1] = Rudolf;
// }
