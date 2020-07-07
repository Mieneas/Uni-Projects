import java.util.InputMismatchException;
import java.util.Stack;

import static java.lang.Math.abs;

public class Board {
    private int n;

    public Board(int n)
    {
        // TODO
    }
    
    
    public int getN() { return n; }

    public int nFreeFields() {
        // TODO
    }

    public int getField(Position pos) throws InputMismatchException
    {
        // TODO
    }

    public void setField(Position pos, int token) throws InputMismatchException
    {
        // TODO
    }

    public void doMove(Position pos, int player)
    {
        // TODO
    }

    public void undoMove(Position pos)
    {
        // TODO
    }

    public boolean isGameWon() {
        // TODO
    }

    public Iterable<Position> validMoves()
    {
        // TODO
    }

    public void print()
    {
        // TODO
    }

}

