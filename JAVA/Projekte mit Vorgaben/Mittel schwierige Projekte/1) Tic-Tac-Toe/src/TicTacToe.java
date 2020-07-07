import java.util.Stack;

public class TicTacToe {

    public static int alphaBetaIm(Board board, int player, int alpha, int beta)
    {
        // TODO
        if(board.isGameWon() && player == 1){
                return player * -(board.nFreeFields() + 1);
        }
        else if(board.isGameWon() && player == -1){
            return player * (board.nFreeFields() + 1);
        }
        else if(!board.isGameWon() && board.nFreeFields() == 0){
            return 0;
        }

        for(Position pos : board.validMoves()){
            board.doMove(pos ,player);
            int score = -alphaBetaIm(board, -player, -beta, -alpha);
            board.undoMove(pos);
            if(score > alpha){
                alpha = score;
                if(alpha >= beta)
                    break;
            }
        }
        return alpha;
    }

    public static int alphaBeta(Board board, int player){
        return alphaBetaIm(board, player, -Integer.MAX_VALUE, Integer.MAX_VALUE);
    }

    public static void evaluatePossibleMoves(Board board, int player)
    {
        // TODO
        int[][] temp = new int[board.getN()][board.getN()];

        for(Position pos : board.validMoves()) {
            board.doMove(pos, player);
            player = -player;
                temp[pos.x][pos.y] = -alphaBeta(board, player);
            board.undoMove(pos);
            player = -player;

            //[board.validMoves().iterator().next().x][board.validMoves().iterator().next().y]
        }
        for(int i = 0; i < board.getN(); i++) {
            for (int j = 0; j < board.getN(); j++) {
                if(board.board[j][i] == 1){
                    System.out.print(" x ");
                }
                else if(board.board[j][i] == -1){
                    System.out.print(" o ");
                }
                else{
                    if(temp[j][i] < 0)
                        System.out.print(temp[j][i] + " ");
                    if(temp[j][i] >= 0)
                        System.out.print(" " + temp[j][i] + " ");
                }
            }
            System.out.println();
        }
    }


    public static void main(String[] args)
    {
        Board board = new Board(3);
        int player = 1;

        //Übungblatt
        board.doMove(new Position(0, 0), 0);
        board.doMove(new Position(1, 0), -1);
        board.doMove(new Position(2, 0), 0);
        board.doMove(new Position(0, 1), 0);
        board.doMove(new Position(1, 1), 0);
        board.doMove(new Position(2, 1), 0);
        board.doMove(new Position(0, 2), 0);
        board.doMove(new Position(1, 2), 0);
        board.doMove(new Position(2, 2), 1);

        evaluatePossibleMoves(board, player);

        //Erwartet:
        //     0  o   3
        //     0  3  -2
        //     3  0   x

        //Test2
        System.out.println();
        player = -1;
        board.doMove(new Position(0, 0), 0);
        board.doMove(new Position(1, 0), 1);
        board.doMove(new Position(2, 0), 0);
        board.doMove(new Position(0, 1), 0);
        board.doMove(new Position(1, 1), 0);
        board.doMove(new Position(2, 1), 0);
        board.doMove(new Position(0, 2), 0);
        board.doMove(new Position(1, 2), 0);
        board.doMove(new Position(2, 2), 0);

        evaluatePossibleMoves(board, player);
        //Erwartet:
        //     0  x  0
        //    -3  0 -3
        //    -3  0 -3

        //Test3
        System.out.println();
        player = -1;
        board.doMove(new Position(0, 0), 0);
        board.doMove(new Position(1, 0), 0);
        board.doMove(new Position(2, 0), 0);
        board.doMove(new Position(0, 1), 0);
        board.doMove(new Position(1, 1), 1);
        board.doMove(new Position(2, 1), 0);
        board.doMove(new Position(0, 2), 0);
        board.doMove(new Position(1, 2), 0);
        board.doMove(new Position(2, 2), 0);

        evaluatePossibleMoves(board, player);
        //Erwartet:
        //     0 -3  0
        //    -3  x -3
        //     0 -3  0

        //Test4
        System.out.println();
        player = -1;
        board.doMove(new Position(0, 0), 0);
        board.doMove(new Position(1, 0), 0);
        board.doMove(new Position(2, 0), 0);
        board.doMove(new Position(0, 1), 0);
        board.doMove(new Position(1, 1), 0);
        board.doMove(new Position(2, 1), 0);
        board.doMove(new Position(0, 2), 0);
        board.doMove(new Position(1, 2), 0);
        board.doMove(new Position(2, 2), 0);

        evaluatePossibleMoves(board, player);
        //Erwartet:
        //     0  0  0
        //     0  0  0
        //     0  0  0

        //Test5
        System.out.println();
        player = 1;
        board.doMove(new Position(0, 0), 0);
        board.doMove(new Position(1, 0), 0);
        board.doMove(new Position(2, 0), 0);
        board.doMove(new Position(0, 1), 0);
        board.doMove(new Position(1, 1), 0);
        board.doMove(new Position(2, 1), 0);
        board.doMove(new Position(0, 2), 0);
        board.doMove(new Position(1, 2), 0);
        board.doMove(new Position(2, 2), 0);

        evaluatePossibleMoves(board, player);
        //Erwartet:
        //     0  0  0
        //     0  0  0
        //     0  0  0

        //test6
        System.out.println();
        player = 1;
        board.doMove(new Position(0, 0), 1);
        board.doMove(new Position(1, 0), 0);
        board.doMove(new Position(2, 0), 1);
        board.doMove(new Position(0, 1), -1);
        board.doMove(new Position(1, 1), 1);
        board.doMove(new Position(2, 1), -1);
        board.doMove(new Position(0, 2), -1);
        board.doMove(new Position(1, 2), 1);
        board.doMove(new Position(2, 2), -1);

        evaluatePossibleMoves(board, player);
        //Erwartet:
        //     x  1  x
        //     o  x  o
        //     o  x  o

        //test7
        System.out.println();
        player = -1;
        board.doMove(new Position(0, 0), 1);
        board.doMove(new Position(1, 0), 0);
        board.doMove(new Position(2, 0), 1);
        board.doMove(new Position(0, 1), 0);
        board.doMove(new Position(1, 1), 1);
        board.doMove(new Position(2, 1), -1);
        board.doMove(new Position(0, 2), 0);
        board.doMove(new Position(1, 2), 0);
        board.doMove(new Position(2, 2), -1);

        evaluatePossibleMoves(board, player);
        //Erwartet:
        //      0  -3  x
        //     -3   x  o
        //     -3  -3  o
    }
}

