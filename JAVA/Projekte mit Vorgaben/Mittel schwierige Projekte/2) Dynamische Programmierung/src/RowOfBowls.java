import java.util.LinkedList;
import java.util.List;

public class RowOfBowls {

    int board[][];
    int Values[];

    public RowOfBowls() {
    }

    public int maxGain(int[] values)
    {
        // TODO
        this.Values = values;
        board = new int[values.length][values.length];
        for(int i = 1; i < values.length; i++){
            for(int j = 0; j < values.length - i; j++){
                if(i == 1) {
                    if (values[j + 1] - values[j] > values[j] - values[j + 1]) {
                        board[i+j][j] = values[j + 1] - values[j];
                    }else { board[i+j][j] = values[j] - values[j + 1]; }
                }
                if(i > 1){
                    if(i + j < values.length) {
                        if (values[i + j] - board[i + j - 1][j] > values[j] - board[i + j][j + 1]) {
                            board[i + j][j] = values[i + j] - board[i + j - 1][j];
                        } else {
                            board[i + j][j] = values[j] - board[i + j][j + 1];
                        }
                    }
                }
            }
        }
        /*for(int i = 0; i < values.length; i++){
            for(int j = 0; j  < values.length; j++){
                if(board[j][i] < 0)
                    System.out.print(board[j][i] + " ");
                if(board[j][i] >= 0)
                    System.out.print(" " + board[j][i] + " ");
            }
            System.out.println();
        }*/
        return board[values.length - 1][0];
    }

    public int maxGainRecursive(int[] values, int left, int right) {
        // TODO
        if(right - left == 0) {
            return values[right];
        }

        int score = maxGainRecursive(values, left, --right);
        right++;
        int x = values[right] - score;

        int y = values[left];
        int score2 = maxGainRecursive(values, ++left, right);
        y = y - score2;
        return Math.max(x, y);
    }

    public int maxGainRecursive(int[] values){
        int score = maxGainRecursive(values, 0, values.length - 1);
        return score;
    }

    public Iterable<Integer> optimalSequence() {
        // TODO
        int first = 0;
        int last = this.Values.length - 1;
        int indexA = 0, indexB = 0;
        int player = 1;
        int[] spieler1 = new int[(this.Values.length / 2) + 1];
        int[] spieler2 = new int[(this.Values.length / 2) + 1];

        List<Integer> sequenz = new LinkedList<>();

        for (int i = 0; i < this.Values.length; i++) {
            if(first == last){
                if(player == 1)
                    spieler1[indexA] = last;
                if(player == -1)
                    spieler2[indexB] = last;
                break;
            }

            if (this.Values[i] - board[last][first + 1] >= this.Values[last] - board[last - 1][i]) {
                if (player == 1) {
                    spieler1[indexA] = first;
                    indexA++;
                } else if (player == -1) {
                    spieler2[indexB] = first;
                    indexB++;
                }
                first++;
                player = -player;
            } else {
                if (player == 1) {
                    spieler1[indexA] = last;
                    indexA++;
                } else if (player == -1) {
                    spieler2[indexB] = last;
                    indexB++;
                }
                last--;
                i = first;
                player = -player;
            }
        }
        indexA = 0;
        indexB = 0;
        for(int i = 0; i < this.Values.length; i++){
            if(i % 2 == 0) {
                sequenz.add(spieler1[indexA]);
                indexA++;
            }
            if(i % 2 != 0) {
                sequenz.add(spieler2[indexB]);
                indexB++;
            }
        }
        return sequenz;
    }


    public static void main(String[] args)
    {
        // for testing
        RowOfBowls test = new RowOfBowls();

        int[] array = new int []{4, 7, 2, 3};

        int result  = test.maxGain(array);
        System.out.println(result);

        result = test.maxGainRecursive(array);
        System.out.println(result);

        Iterable<Integer> a = test.optimalSequence();
        for(int t : a){
            System.out.print(t + " ");
        }
    }
}

