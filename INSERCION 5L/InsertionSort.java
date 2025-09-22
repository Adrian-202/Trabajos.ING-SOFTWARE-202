public class InsertionSort {
    public static void insertionSort(int[] arr) {
        for (int i = 1; i < arr.length; i++) {
            int temp = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > temp) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = temp;
        }
    }

    public static void main(String[] args) {
        int[] arr = {70, 15, 2, 51, 60};

        System.out.print("Antes: ");
        for (int x : arr) System.out.print(x + " ");
        System.out.println();

        insertionSort(arr);

        System.out.print("Después: ");
        for (int x : arr) System.out.print(x + " ");
        System.out.println();
    }
}
