public class Genomics {

   public static long optBottomUp(String strang, String[] dictionary) {
        return 0;
    }



    public static long opt(String strang, String[] worterbuch) {

        long Anzahl= 0;

        for (int i = 0; i < worterbuch.length; i++) {
            String genom = worterbuch[i];
            if (strang.length() < genom.length() && strang.startsWith(genom, 0)) {
                return Anzahl;
            } 
if (strang.length() == genom.length() && strang.startsWith(genom, 0)) {
                return 1;
            } else {
                if (strang.startsWith(genom, 0)) {
                    int subStringLen = genom.length();
                    String newStrang = strang.substring(subStringLen);
                    genom = genom + Genomics.opt(newStrang, worterbuch);
                }
            }
        }
        return Anzahl;
    }

    public static void main(String[] args) {

    }
}

