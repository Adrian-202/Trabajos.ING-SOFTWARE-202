using System;

class Program {
    static void OrganizarDatos(int[] coleccion) {
        int total = coleccion.Length;
        bool repetir;
        int ciclo = 0;
        do {
            repetir = false;
            for (int idx = 0; idx < total - 1 - ciclo; idx++) {
                if (coleccion[idx] > coleccion[idx + 1]) {
                    int mamo = coleccion[idx];
                    coleccion[idx] = coleccion[idx + 1];
                    coleccion[idx + 1] = mamo;
                    repetir = true;
                }
            }
            ciclo++;
        } while (repetir);
    }

    static void Main() {
        int[] mis_datos = { 1, 4, 2, 3, 5, 8, 6 };
        OrganizarDatos(mis_datos);
        foreach (int x in mis_datos) Console.Write(x + " ");
    }
}