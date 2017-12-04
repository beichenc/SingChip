/**
 * Created by beichenchen on 2017-12-03.
 */
public class Main {
    public static void main(String[] args) {

        //SineAndHanningGenerator generator = new SineAndHanningGenerator();

        //generator.sine();
        //generator.hanning();
        //generator.log2nToDouble();

        PlayWav player = new PlayWav();

        try {
            player.play();
        } catch (Exception e){
            System.out.println(e.getMessage());
        }
    }
}
