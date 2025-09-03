using System;

class Program
{
    static void Main()
    {
        // Declaramos un arreglo de números enteros
        int[] numeros = { 10, 20, 30, 40, 50 };

        // Recorremos el arreglo con un for
        for (int i = 0; i < numeros.Length; i++)
        {
            Console.WriteLine("Número en la posición " + i + ": " + numeros[i]);
        }

        // También podemos calcular la suma de todos los números
        int suma = 0;
        foreach (int n in numeros)
        {
            suma += n;
        }
        Console.WriteLine("La suma de todos los números es: " + suma);
    }
}
