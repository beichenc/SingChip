// #include <math.h>
//
// int A4 = 440;
// double C0 = A4*pow(2, -4.75);
// char name[] = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
//
// char* freqToTone(short frequency) {
//     // TODO: Kolla om vi tar log2 av double eller int
//     int h = 12*log(frequency/C0)/log(2);
//     int octave = h/12;
//     int n = h % 12;
//     char tone[] = name[n];
//     int size = sizeof(tone)/sizeof(char);
//     tone[size] = octave + '0';
//     return tone;
// }
