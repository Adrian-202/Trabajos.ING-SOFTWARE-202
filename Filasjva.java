public class Main {
    public static void main(String[] args) {
        int f = 3, c = 3;
        int[][] twoDarr = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        };

        System.out.println("RECORRIDO DE FILAS");

        for (int i = 0; i < f; i++) {
            System.out.print("Fila " + (i+1) + ": ");
            for (int j = 0; j < c; j++) {
                System.out.print(twoDarr[i][j] + " ");
            }
            System.out.println();
        }
    }
}
