using System;

class Program {
    static void Main() {
        int f = 3, c = 3;
        int[,] twoDarr = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        };

        Console.WriteLine("PROGRAMA DE RECORRIDO DE COLUMNAS");

        for (int j = 0; j < c; j++) {
            Console.Write("Columna " + (j+1) + ": ");
            for (int i = 0; i < f; i++) {
                Console.Write(twoDarr[i, j] + " ");
            }
            Console.WriteLine();
        }
    }
}
