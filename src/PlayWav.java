import java.io.*;
import java.util.concurrent.TimeUnit;

import sun.audio.*;

/**
 * A simple Java sound file example (i.e., Java code to play a sound file).
 * AudioStream and AudioPlayer code comes from a javaworld.com example.
 * @author alvin alexander, devdaily.com.
 */
public class PlayWav
{
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
}