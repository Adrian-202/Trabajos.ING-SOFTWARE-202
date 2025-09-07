using System;
using System.Collections.Generic;

class Recorrido {
    static void Main() {
        List<int> arr = new List<int> { 10, 20, 30 };
        Console.WriteLine("Recorrido:");
        foreach (int num in arr) {
            Console.WriteLine(num);
        }
    }
}
