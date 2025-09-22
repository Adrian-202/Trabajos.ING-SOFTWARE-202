using System;

class Program {
    static void InsertionSort(int[] arr) {
        for (int i = 1; i < arr.Length; i++) {
            int temp = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > temp) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = temp;
        }
    }

    static void Main() {
        int[] arr = {70, 15, 2, 51, 60};
        Console.WriteLine("Antes: " + string.Join(" ", arr));
        InsertionSort(arr);
        Console.WriteLine("Después: " + string.Join(" ", arr));
    }
}
