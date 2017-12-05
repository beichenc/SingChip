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

        try {
            player.play();
        } catch (Exception e) {

        }
    }
}
