/************************************************************************
	fft.h

    FFT Audio Analysis
    Copyright (C) 2011 Simon Inns

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Email: simon.inns@gmail.com

************************************************************************/

// Enhanced: Beichen Chen and Amy Chen 4 Dec 2017

#ifndef _FFT_H
#define _FFT_H

// Definitions
#define N_WAVE      1024    // full length of Sinewave[]
#define LOG2_N_WAVE 10      // log2(N_WAVE)

// Function prototypes
void fix_fft(short fr[], short fi[], short m);
int gst_spectrum_fix_fft (short fr[], short fi[], int m, int inverse);
extern short gst_spectrum_Sinewave[N_WAVE];     /* placed at end of this file for clarity */
extern double hanning[N_WAVE];     /* placed at end of this file for clarity */
static short gst_spectrum_fix_mpy (short a, short b);

#endif
