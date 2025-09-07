using System;
using System.Collections.Generic;

class Busqueda {
    static void Main() {
        List<int> arr = new List<int> { 10, 15, 20, 30 };
        int buscado = 20;
        int indice = arr.IndexOf(buscado);

        if (indice != -1) {
            Console.WriteLine(buscado + " encontrado en índice " + indice);
        } else {
            Console.WriteLine(buscado + " no encontrado");
        }
    }
}

