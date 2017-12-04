import java.util.ArrayList;

/**
 * Created by beichenchen on 2017-12-03.
 */
public class SineAndHanningGenerator {

    private ArrayList<Integer> sineCurve2;
    private ArrayList<Integer> hanning;
    private ArrayList<Double> hanningD;

    public SineAndHanningGenerator() {

    }

    public void sine() {
        sineCurve2 = new ArrayList<>();

        for (int i = 0; i < 2048; i++) {
            double value = Math.sin(2*Math.PI/2048*i)*65536/2;
            sineCurve2.add((int)value);
        }
        System.out.println(sineCurve2);
    }

    public void hanning() {
        hanning = new ArrayList<>();

        for (int i = 0; i < 1024; i++) {
            double value = 0.5*(1-Math.cos(2*Math.PI*i/1023))*65536/2;
            hanning.add((int)value);
        }
        System.out.println(hanning);
    }

    public void hanningDouble() {
        hanningD = new ArrayList<>();


        for (int i = 0; i < 1024; i++) {
            double value = Math.round(0.5*(1-Math.cos(2*Math.PI*i/1023))*1000000000.0)/1000000000.0;
            hanningD.add(value);
            if (i % 10 == 0) {
                System.out.printf("\n%.10f,", value);
            } else {
                System.out.printf("%.10f" + ",", value);
            }
        }
    }

    public void log2nToDouble() {
        for (int i = 0; i < 12; i++) {
            double j = i*1.0/12.0;
            double value = Math.pow(2.0,j);
            System.out.printf("%.10f"+",", value);
        }
    }
}
