import java.util.HashMap;

/**
 * Created by beichenchen on 2017-12-06.
 */
public class Song {

    private String[] tones;
    private float[] volumes;
    private HashMap<String, Float> tonesMap;

    public Song(String[] tones, float[] volumes) {
        this.tones = tones;
        this.volumes = volumes;
        int numberOfTones = 41;
        float frequenciesLibrary[] = new float[]{195.998F, 207.652F, 220.000F, 233.082F, 246.942F, 261.626F, 277.183F, 293.665F,
                311.127F, 329.628F, 349.228F, 369.994F, 391.995F, 415.305F, 440.000F, 466.164F, 493.883F, 523.251F, 554.365F, 587.330F,
                622.254F, 659.255F, 698.456F, 739.989F, 783.991F, 830.609F, 880.000F, 932.328F, 987.767F, 1046.50F, 1108.73F, 1174.66F,
                1244.51F, 1318.51F, 1396.91F, 1479.98F, 1567.98F, 1661.22F, 1760.00F, 1864.66F, 1975.53F};
        String tonesLibrary[] = new String[]{"G3", "G#3", "A3", "A#3", "B3", "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4", "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5", "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6"};
        tonesMap = new HashMap<>();
        for (int i = 0; i < numberOfTones; i++) {
            tonesMap.put(tonesLibrary[i],frequenciesLibrary[i]);
        }
    }

    public float[] getFrequencyList() {
        float frequencyList[] = new float[tones.length];
        for (int i = 0; i < tones.length; i++) {
            frequencyList[i] = tonesMap.get(tones[i]);
        }
        return frequencyList;
    }

    public float[] getVolumeList() {
        return volumes;
    }
}
