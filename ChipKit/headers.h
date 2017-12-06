/* headers.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Update 2015-08-28 by F Lundevall
   Modified by Beichen Chen and Amy Chen 4 Dec 2017

   For copyright and licensing, see file COPYING */

// Display related functions from display.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);
char * itoaconv( int num );

// Tony.c
void tony(void);

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

// Declare bitmap array containing font
extern const uint8_t const font[128*8];
// Declare bitmap array containing icon
extern const uint8_t const icon[128];
// Declare text buffer for display output
extern char textbuffer[4][16];

// Written as part of asm lab: delay, time2string
void delay(int);
void time2string( char *, int );
// Written as part of i/o lab: getbtns, getsw, enable_interrupt
int getbtns(void);
int getsw(void);
void enable_interrupt(void);

// Song library
#define MAX_NAME_LENGTH (9)
#define MAX_SONG_LENGTH (10)
#define SONG_LIBRARY_SIZE (10)
extern char blinka[12][9];
extern char rudolf[12][9];
extern char songLibrary[10][12][9];
