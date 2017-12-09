/**
 * Created by beichenchen on 2017-12-03.
 */
public class Main {
    public static void main(String[] args) {

        //SineAndHanningGenerator generator = new SineAndHanningGenerator();

        //generator.sine();
        //generator.hanning();
        //generator.log2nToDouble();

        //PlayWav player = new PlayWav();

        /*try {
            //player.play();
            player.playWithVolumeControl();
        } catch (Exception e){
            System.out.println(e.getMessage());
        }*/

        OscillatorPlayer player = new OscillatorPlayer();
        Song blinka = new Song(new String[]{"C4","C4","G4","G4","A4","A4","G4"}, new float[]{0,0,0,0,-4.0F,-4.0F,0});
        Song rudolf = new Song(new String[]{"G4", "A4", "G4", "E4", "C5", "A4", "G4"}, new float[]{0,0,0,0,-5.0F,0,0});
        Song elise = new Song(new String[]{"E5","D#5","E5","D#5","E5","B4","D5","C5","A4"}, new float[]{-6.0F,-6.0F,-6.0F,-6.0F,-6.0F,-4.0F,-5.0F,-4.0F,-4.0F});
        Song birthday = new Song(new String[]{"C4","C4","D4","C4","F4","E4"}, new float[]{0,0,0,0,0,0});

        try {
            player.play(birthday);
        } catch (Exception e) {

        }
    }
}
