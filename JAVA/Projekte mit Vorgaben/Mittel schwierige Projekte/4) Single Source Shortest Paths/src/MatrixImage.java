import java.io.*;
import java.util.InputMismatchException;
import java.util.LinkedList;
import java.util.Random;
import java.util.Scanner;

public class MatrixImage implements Image {
    public int[][] field;

    public MatrixImage(int sx, int sy) {
        field = new int[sx][sy];
    }

    // copy constructor
    public MatrixImage(MatrixImage that) {
        this(that.sizeX(), that.sizeY());
        for (int x = 0; x < sizeX(); x++) {
            field[x] = that.field[x].clone();
        }
    }

    public MatrixImage(String filename) throws java.io.FileNotFoundException {
        System.setIn(new FileInputStream(filename));
        Scanner in = new Scanner(System.in);
        int sx = in.nextInt();
        int sy = in.nextInt();
        field = new int[sx][sy];
        for (int y = 0; y < sy; y++) {
            for (int x = 0; x < sx; x++) {
                field[x][y] = in.nextInt();
            }
        }
    }

    @Override
    public int sizeX() {
        return field.length;
    }

    @Override
    public int sizeY() {
        return field[0].length;
    }

    @Override
    public double contrast(Coordinate p0, Coordinate p1) throws InputMismatchException {
        // TODO
        if(p0.x > this.sizeX() - 1 || p0.x < 0 || p0.y > this.sizeY() - 1 || p0.y < 0){
            throw new InputMismatchException("der Vertex 'p0' ist außerhalb der Matrix");
        }

        if(p1.x > this.sizeX() - 1 || p0.x < 0 || p1.y > this.sizeY() - 1 || p1.y < 0){
            throw new InputMismatchException("der Vertex 'p1' ist außerhalb der Matrix");
        }

        return Math.abs(this.field[p0.x][p0.y] - this.field[p1.x][p1.y]);
    }

    @Override
    public void removeVPath(int[] path) {
        // TODO
        int k = 0;
        MatrixImage new_matrix = new MatrixImage(this.sizeX() - 1, this.sizeY());
        for(int i = 0; i < this.sizeY(); i++){
            for(int j = 0; j < this.sizeX(); j++){
                if(j != path[i]){
                    new_matrix.field[j - k][i] = this.field[j][i];
                }
                if(j == path[i]){
                    k = 1;
                }
            }
            k = 0;
        }
        this.field = new_matrix.field;
    }

    @Override
    public String toString() {
        String str = "";
        for (int y = 0; y < sizeY(); y++) {
            for (int x = 0; x < sizeX(); x++) {
                str += field[x][y] + " ";
            }
            str += "\n";
        }
        return str;
    }

    @Override
    public void render() {
        System.out.println(toString());
    }

    public static void main(String [] args){
        MatrixImage matrix = new MatrixImage(3 , 3);
        int k = 0;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                matrix.field[i][j] = i + j + k;
            }
            k++;
        }
        Coordinate cor = new Coordinate(1, 1);
        Coordinate cor1 = new Coordinate(2,1);
        double contr = matrix.contrast(cor, cor1);
        System.out.println("contrast von: " + "(" + matrix.field[cor.x][cor.y] + "," + matrix.field[cor1.x][cor1.y] + ") = " + contr);
        int[] path = new int[3];
        path[0] = 4;
        path[1] = 3;
        path[2] = 2;
        System.out.println("Vor dem Löschen");
        System.out.println(matrix);
        matrix.removeVPath(path);
        System.out.print("gelöscht muss: ");
        for(int i = 0; i < path.length; i++) {
            System.out.print(path[i] + " ");
        }
        System.out.println("\nNach dem Löschen");
        System.out.println(matrix);
    }

}

