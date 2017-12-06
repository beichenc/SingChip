/* tonetable.c

   This file written 2017 by Beichen Chen and Amy Chen

   For copyright and licensing, see file COPYING */
// TODO: Check copyrights, I mean, there's absolutely no code in this file not written by us only.

#include <stdint.h>   /* Declarations of uint_32 and the like */

char *name[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

// Function found online but own complementary functions below needed to extract decimal value of needed precision.
unsigned int Log2nInt(unsigned int n)
{
   return (n > 1)? 1 + Log2nInt(n/2): 0;
}

double abs(double input) {
    if (input < 0.0) {
        return input * ((double)-1);
    }
    return input;
}

// Values generated in Java
double log2nList[12] = {1.0000000000,1.0594630944,1.1224620483,1.1892071150,1.2599210499,
    1.3348398542,1.4142135624,1.4983070769,1.5874010520,1.6817928305,1.7817974363,1.8877486254};

double log2nD(double input) {
    int log2nInt = Log2nInt(round(input));
    double log2nIntD = (double)log2nInt;
    double power = (double)(1 << log2nInt); // 2^(log2nInt)
    double diff = input/power;
    int i;
    double min;
    min = abs(diff-log2nList[0]);
    int minIndex = 0;
    for (i = 1; i < 12; i++) {
        double absdiff = abs(diff-log2nList[i]);
        double test = absdiff*100.0;
        if (absdiff < min) {
            min = absdiff;
            minIndex = i;
        }
    }
    return log2nIntD + (double)minIndex/12.0;
}

int round(double x)
{
    return (int)(x + 0.5);
}

int floor(double x) {
    return x/1;
}

void freqToTone(short frequency, char* tone) {
    double frequencyD = (double)frequency;
    // A4 = 440;
    // C0 = 16.3515978313 //440*pow(2, -4.75);
    double hd = 12.0*log2nD(frequencyD/16.3515978313);
    int h = round(hd);
    int octave = h/12;
    int n = h % 12;
    if (n == 0 || n == 2 || n == 4 || n == 5 || n == 7 || n == 9 || n == 11) {
        tone[0] = name[n][0];
        tone[1] = octave + '0';
        // TODO: Set correct value same as in songLibrary
        tone[2] = '\0';
    } else {
        tone[0] = name[n][0];
        tone[1] = name[n][1];
        tone[2] = octave + '0';
    }
}
