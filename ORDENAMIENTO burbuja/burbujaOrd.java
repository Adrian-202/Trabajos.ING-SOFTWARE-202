public class burbujaOrd {
    public static void burbujaOrd(int[] arr) {
        int n = arr.length;
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    int temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }

    public static void main(String[] args) {
        int[] arr = {70, 15, 2, 51, 60};

        System.out.print("Antes: ");
        for (int x : arr) System.out.print(x + " ");
        System.out.println();

        burbujaOrd(arr);

        System.out.print("Después: ");
        for (int x : arr) System.out.print(x + " ");
        System.out.println();
    }
}
