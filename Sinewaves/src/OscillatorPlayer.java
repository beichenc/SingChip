/*
 *	OscillatorPlayer.java
 *
 *	This file is part of jsresources.org
 */

/*
 * Copyright (c) 1999 -2001 by Matthias Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
|<---            this code is formatted to fit into 80 columns             --->|
*/

import java.io.IOException;
import java.util.HashMap;
import java.util.concurrent.TimeUnit;

import javax.sound.sampled.*;

/*	If the compilation fails because this class is not available,
	get gnu.getopt from the URL given in the comment below.
*/

/**	<titleabbrev>OscillatorPlayer</titleabbrev>
 <title>Playing waveforms</title>

 <formalpara><title>Purpose</title>
 <para>
 Plays waveforms (sine, square, ...).
 </para></formalpara>

 <formalpara><title>Usage</title>
 <para>
 <cmdsynopsis>
 <command>java OscillatorPlayer</command>
 <arg><option>-t <replaceable>waveformtype</replaceable></option></arg>
 <arg><option>-f <replaceable>signalfrequency</replaceable></option></arg>
 <arg><option>-r <replaceable>samplerate</replaceable></option></arg>
 <arg><option>-a <replaceable>amplitude</replaceable></option></arg>
 </cmdsynopsis>
 </para>
 </formalpara>

 <formalpara><title>Parameters</title>
 <variablelist>
 <varlistentry>
 <term><option>-t <replaceable>waveformtype</replaceable></option></term>
 <listitem><para>the waveform to play. One of sine, sqaure, triangle and sawtooth. Default: sine.</para></listitem>
 </varlistentry>
 <varlistentry>
 <term><option>-f <replaceable>signalfrequency</replaceable></option></term>
 <listitem><para>the frequency of the signal to create. Default: 1000 Hz.</para></listitem>
 </varlistentry>
 <varlistentry>
 <term><option>-r <replaceable>samplerate</replaceable></option></term>
 <listitem><para>the sample rate to use. Default: 44.1 kHz.</para></listitem>
 </varlistentry>
 <varlistentry>
 <term><option>-a <replaceable>amplitude</replaceable></option></term>
 <listitem><para>the amplitude of the generated signal. May range from 0.0 to 1.0. 1.0 means a full-scale wave. Default: 0.7.</para></listitem>
 </varlistentry>
 </variablelist>
 </formalpara>

 <formalpara><title>Bugs, limitations</title>
 <para>
 Full-scale waves can lead to clipping. It is currently not known
 which component is responsible for this.
 </para></formalpara>

 <formalpara><title>Source code</title>
 <para>
 <ulink url="OscillatorPlayer.java.html">OscillatorPlayer.java</ulink>,
 <ulink url="Oscillator.java.html">Oscillator.java</ulink>,
 <ulink url="http://www.urbanophile.com/arenn/hacking/download.html">gnu.getopt.Getopt</ulink>
 </para></formalpara>

 */
public class OscillatorPlayer
{
    private static final int	BUFFER_SIZE = 128000;
    private static boolean		DEBUG = false;
    private HashMap<String, Float> tonesMap;

    public OscillatorPlayer() {

        int numberOfTones = 41;
        float frequencies[] = new float[]{195.998F, 207.652F, 220.000F, 233.082F, 246.942F, 261.626F, 277.183F, 293.665F,
                311.127F, 329.628F, 349.228F, 369.994F, 391.995F, 415.305F, 440.000F, 466.164F, 493.883F, 523.251F, 554.365F, 587.330F,
                622.254F, 659.255F, 698.456F, 739.989F, 783.991F, 830.609F, 880.000F, 932.328F, 987.767F, 1046.50F, 1108.73F, 1174.66F,
                1244.51F, 1318.51F, 1396.91F, 1479.98F, 1567.98F, 1661.22F, 1760.00F, 1864.66F, 1975.53F};
        String tones[] = new String[]{"G3", "G#3", "A3", "A#3", "B3", "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4", "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5", "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6"};
        System.out.println(frequencies.length);
        System.out.println(tones.length);
        tonesMap = new HashMap<>();
        for (int i = 0; i < numberOfTones; i++) {
            tonesMap.put(tones[i],frequencies[i]);
        }
    }

    public float[] getFrequencyList(String[] tonesList) {
        float frequencyList[] = new float[tonesList.length];
        for (int i = 0; i < tonesList.length; i++) {
            frequencyList[i] = tonesMap.get(tonesList[i]);
        }
        return frequencyList;
    }

    public void play() throws Exception
    {
        // TODO: make song library
        float blinka[] = getFrequencyList(new String[]{"C4","C4","G4","G4","A4","A4","G4"});

        float rudolf[] = getFrequencyList(new String[]{"G4", "A4", "G4", "E4", "C5", "A4", "G4"});
        // TODO: finish volume
        float rudolfVolume[] = new float[]{0,0,0,0,-4.0F,0,0};

        byte[]		abData;
        AudioFormat	audioFormat;
        int	nWaveformType = Oscillator.WAVEFORM_SINE;
        float	fSampleRate = 44100.0F;
        float	fSignalFrequency = 390.0F;
        float	fAmplitude = 0.7F;

        for (int i = 0; i < blinka.length; i++) {

            /*
             *	Parsing of command-line options takes place...
             */
            String args[] = new String[]{};
            Getopt	g = new Getopt("AudioPlayer", args, "ht:r:f:a:D");
            int	c;
            while ((c = g.getopt()) != -1)
            {
                switch (c)
                {
                    case 'h':
                        printUsageAndExit();

                    case 't':
                        nWaveformType = getWaveformType(g.getOptarg());
                        break;

                    case 'r':
                        fSampleRate = Float.parseFloat(g.getOptarg());
                        break;

                    case 'f':
                        fSignalFrequency = Float.parseFloat(g.getOptarg());
                        break;

                    case 'a':
                        fAmplitude = Float.parseFloat(g.getOptarg());
                        break;

                    case 'D':
                        DEBUG = true;
                        break;

                    case '?':
                        printUsageAndExit();

                    default:
                        if (DEBUG) { out("getopt() returned " + c); }
                        break;
                }
            }

            System.out.println(blinka[i]);

            audioFormat = new AudioFormat(AudioFormat.Encoding.PCM_SIGNED,
                    fSampleRate, 16, 2, 4, fSampleRate, false);
            AudioInputStream	oscillator = new Oscillator(
                    nWaveformType,
                    blinka[i],
                    fAmplitude,
                    audioFormat,
                    AudioSystem.NOT_SPECIFIED);
            SourceDataLine	line = null;
            DataLine.Info	info = new DataLine.Info(
                    SourceDataLine.class,
                    audioFormat);
            try
            {
                line = (SourceDataLine) AudioSystem.getLine(info);
                line.open(audioFormat);
            }
            catch (LineUnavailableException e)
            {
                e.printStackTrace();
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }

            FloatControl gainControl =
                    (FloatControl) line.getControl(FloatControl.Type.MASTER_GAIN);
            // Rudolf
            /*if (i == 4) {
                gainControl.setValue(-5.0f); // Reduce volume by 10 decibels.
            }*/

            // Blinka
            if (i == 4 || i == 5) {
                gainControl.setValue(-4.0f); // Reduce volume by 10 decibels.
            }

            line.start();


            abData = new byte[BUFFER_SIZE];
            long startTime = System.currentTimeMillis(); //fetch starting time
            while((System.currentTimeMillis()-startTime)<1000)
            {
                if (DEBUG) { out("OscillatorPlayer.main(): trying to read (bytes): " + abData.length); }
                int	nRead = oscillator.read(abData);
                if (DEBUG) { out("OscillatorPlayer.main(): in loop, read (bytes): " + nRead); }
                int	nWritten = line.write(abData, 0, nRead);
                if (DEBUG) { out("OscillatorPlayer.main(): written: " + nWritten); }
            }
            line.stop();
            TimeUnit.MILLISECONDS.sleep(500);
        }
    }


    private static int getWaveformType(String strWaveformType)
    {
        int	nWaveformType = Oscillator.WAVEFORM_SINE;
        strWaveformType = strWaveformType.trim().toLowerCase();
        if (strWaveformType.equals("sine"))
        {
            nWaveformType = Oscillator.WAVEFORM_SINE;
        }
        else if (strWaveformType.equals("square"))
        {
            nWaveformType = Oscillator.WAVEFORM_SQUARE;
        }
        else if (strWaveformType.equals("triangle"))
        {
            nWaveformType = Oscillator.WAVEFORM_TRIANGLE;
        }
        else if (strWaveformType.equals("sawtooth"))
        {
            nWaveformType = Oscillator.WAVEFORM_SAWTOOTH;
        }
        return nWaveformType;
    }



    private static void printUsageAndExit()
    {
        out("OscillatorPlayer: usage:");
        out("\tjava OscillatorPlayer [-t <waveformtype>] [-f <signalfrequency>] [-r <samplerate>]");
        System.exit(1);
    }


    private static void out(String strMessage)
    {
        System.out.println(strMessage);
    }
}



/*** OscillatorPlayer.java ***/
