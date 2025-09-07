using System;
using System.Collections.Generic;

class Insercion {
    static void Main() {
        List<int> arr = new List<int> { 10, 20, 30 };
        arr.Insert(1, 15);  // Insertar en posición 1
        Console.WriteLine("Después de insertar 15: " + string.Join(", ", arr));
    }
}

