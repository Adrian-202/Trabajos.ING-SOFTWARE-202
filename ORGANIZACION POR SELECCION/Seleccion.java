public class Seleccion {
    public static void seleccion(int[] arr) {
        int n = arr.length;
        for (int i = 0; i < n - 1; i++) {
            int minIdx = i;
            for (int j = i + 1; j < n; j++) {
                if (arr[j] < arr[minIdx]) minIdx = j;
            }
            if (minIdx != i) {
                int tmp = arr[i];
                arr[i] = arr[minIdx];
                arr[minIdx] = tmp;
            }
        }
    }

    public static void main(String[] args) {
        int[] a = {64, 25, 12, 22, 11};
        seleccion(a);
        for (int v : a) System.out.print(v + " ");
        
    }
}
