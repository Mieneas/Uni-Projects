import com.sun.source.tree.WhileLoopTree;

import java.util.InputMismatchException;
import java.util.Iterator;
import java.util.Stack;

import static java.lang.Math.abs;

public class Board {
    private int n;
    public int[][] board;
    int freeFields;

    public Board(int n) throws InputMismatchException
    {
        // TODO
        if(n < 1  || n > 10){
            throw new InputMismatchException("The number must be between 1 and 10");
            }

        this.n = n;
        this.board = new int[this.n][this.n];
    }
    
    
    public int getN() { return n; }

    public int nFreeFields() {
        // TODO
        freeFields = 0;
        for(int i = 0; i < this.n; i++){
            for(int j = 0; j < this.n; j++){
                if(this.board[i][j] == 0){
                    this.freeFields++;
                }
            }
        }
        return freeFields;
    }

    public int getField(Position pos) throws InputMismatchException
    {
        // TODO
        if(pos.x < 0  || pos.y < 0 || pos.x > n - 1 || pos.y > n - 1){
            throw new InputMismatchException("he postion is not exist");
        }

        return this.board[pos.x][pos.y];
    }

    public void setField(Position pos, int token) throws InputMismatchException
    {
        // TODO
        //token: 0 = freeField, -1 = o, 1 = x
        if(pos.x < 0  || pos.y < 0 || pos.x > n - 1 || pos.y > n - 1){
            throw new InputMismatchException("The postion is not exist");
        }
        else if(token != 1 && token != -1 && token != 0){
            throw new InputMismatchException("The tocken can just be -1, 1, or 0");
         }
        this.board[pos.x][pos.y] = token;
    }

    public void doMove(Position pos, int player)
    {
        // TODO
        //player 1 = x, player -1 = o

        this.setField(pos, player);
    }

    public void undoMove(Position pos)
    {
        // TODO
        this.setField(pos, 0);
    }

    public boolean isGameWon() {
        // TODO
        int[] diagonal = new int[8];//für x: 0 -> Diagonal (oben links -> unten rechts), 1 -> anderer Diagonal, 2 -> Waggenrecht, 3 -> Senkecht
        //für o: 4 -> Diagonal (oben links -> unten rechts), 5 -> anderer Diagonal, 6 -> Waggenrecht, 7 -> Senkecht.


        for (int i = 0; i < this.n; i++) {

            //für x
            if (board[i][i] == 1) {//Diagonal (oben links -> unten rechts)_0
                diagonal[0]++;
            }

            if (board[this.n - i - 1][i] == 1) {//anderer Diagonal_1
                diagonal[1]++;
            }

            //für o
            if (board[i][i] == -1) {//Diagonal (oben links -> unten rechts)_4
                diagonal[4]++;
            }

            if (board[this.n - i - 1][i] == -1) {//anderer Diagonal_5
                diagonal[5]++;
            }

            for (int j = 0; j < this.n; j++) {

                //für x
                if (board[j][i] == 1) {//Waggenrecht_2
                    diagonal[2]++;
                }

                if (board[i][j] == 1) {//Senkecht_3
                    diagonal[3]++;
                }

                //für o
                if (board[j][i] == -1) {//Waggenrecht_6
                    diagonal[6]++;
                }

                if (board[i][j] == -1) {//Senkecht_7
                    diagonal[7]++;
                }
            }
            if (diagonal[2] == this.n || diagonal[3] == this.n || diagonal[6] == this.n || diagonal[7] == this.n) {
                return true;
            }
            else if (diagonal[0] == this.n || diagonal[1] == this.n) {
                return true;
            }
            else if (diagonal[4] == this.n || diagonal[5] == this.n) {
                return true;
            } else if (i == this.n - 1){
                    return false;
            }
            diagonal[2] = 0;
            diagonal[3] = 0;
            diagonal[6] = 0;
            diagonal[7] = 0;
        }
        return false;
    }

    public Iterable<Position> validMoves()
    {
        // TODO
        Stack<Position> position = new Stack<>();
            for (int i = 0; i < this.n; i++) {
                for (int j = 0; j < this.n; j++) {
                    if (this.board[i][j] == 0) {
                        position.push(new Position(i, j));
                    }
                }
            }


        Iterable<Position> fields = (Iterable<Position>) position;

        return  fields;
    }

    public void print()
    {
        // TODO
        for(int i = 0; i < this.n; i++){
            for(int j = 0; j < this.n; j++){
                if(board[j][i] == -1) {
                    System.out.print("o");
                    System.out.print("|");
                }else if(board[j][i] == 0){
                    System.out.print(" ");
                    System.out.print("|");
                }else if(board[j][i] == 1){
                    System.out.print("x");
                    System.out.print("|");
                }
            }
            System.out.println();
        }
    }

    public static void main(String args[]){
        Board board = new Board(3);

        board.doMove(new Position(0, 0), 0);
        board.doMove(new Position(1, 0), -1);
        board.doMove(new Position(2, 0), -1);
        board.doMove(new Position(0, 1), 0);
        board.doMove(new Position(1, 1),  1);
        board.doMove(new Position(2, 1), -1);
        board.doMove(new Position(0, 2), 1);
        board.doMove(new Position(1, 2), -1);
        board.doMove(new Position(2, 2), 1);

        boolean win = board.isGameWon();
        board.print();
        System.out.println(win);

        Stack<Position> position = new Stack<>();
        Iterable<Position> fields1 = (Iterable<Position>) position;
        fields1 = board.validMoves();
        for(Position a : fields1){
            System.out.println("(" + a.x + "," + a.y + ")");
        }
        System.out.println(board.nFreeFields());
    }

}

