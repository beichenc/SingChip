import java.io.*;
import java.util.concurrent.TimeUnit;

import sun.audio.*;

import javax.sound.sampled.*;

/**
 * A simple Java sound file example (i.e., Java code to play a sound file).
 * AudioStream and AudioPlayer code comes from a javaworld.com example.
 * @author alvin alexander, devdaily.com.
 */
public class PlayWav
{
    // 9 bars på volym
    public void play()
            throws Exception
    {
        // open the sound file as a Java input stream
        String blinka[] = new String[]{"src/C4.wav","src/C4.wav","src/G4.wav","src/G4.wav","src/A4.wav","src/A4.wav","src/G4.wav"};

        for (int i = 0; i < blinka.length; i++) {
            String gongFile = blinka[i];
            InputStream in = new FileInputStream(gongFile);

            // create an audiostream from the inputstream
            AudioStream audioStream = new AudioStream(in);

            // play the audio clip with the audioplayer class
            AudioPlayer.player.start(audioStream);
            TimeUnit.MILLISECONDS.sleep(1000);
            AudioPlayer.player.stop(audioStream);
            TimeUnit.MILLISECONDS.sleep(500);
        }
    }


    public void playWithVolumeControl() throws Exception {
        String blinka[] = new String[]{"src/C4.wav","src/C4.wav","src/G4.wav","src/G4.wav","src/A4.wav","src/A4.wav","src/G4.wav"};

        for (int i = 0; i < blinka.length; i++) {
            AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(new File(blinka[i]));
            Clip clip = AudioSystem.getClip();
            clip.open(audioInputStream);
            FloatControl gainControl =
                    (FloatControl) clip.getControl(FloatControl.Type.MASTER_GAIN);
            if (i == 4 || i == 5) {
                gainControl.setValue(-4.0f); // Reduce volume by 10 decibels.
            }
            clip.start();
            TimeUnit.MILLISECONDS.sleep(1000);
            clip.stop();
            TimeUnit.MILLISECONDS.sleep(500);
        }


    }

}