public class sort {
    public static void procesarBurbuja(int[] arreglo) {
        int n = arreglo.length;
        boolean movido;
        int vuelta = 0;
        do {
            movido = false;
            for (int i = 0; i < n - 1 - vuelta; i++) {
                if (arreglo[i] > arreglo[i + 1]) {
                    int aux = arreglo[i];
                    arreglo[i] = arreglo[i + 1];
                    arreglo[i + 1] = aux;
                    movido = true;
                }
            }
            vuelta++;
        } while (movido);
    }

    public static void main(String[] args) {
        int[] lista = {1, 4, 2, 3, 5, 8, 6};
        procesarBurbuja(lista);
        for (int v : lista) System.out.print(v + " ");
    }
}