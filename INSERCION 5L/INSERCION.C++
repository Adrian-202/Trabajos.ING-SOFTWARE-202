#include <iostream>
using namespace std;

void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int temp = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

int main() {
    int arr[] = {70, 15, 2, 51, 60};
    int n = sizeof(arr) / sizeof(arr[0]);

    cout << "Antes: ";
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;

    insertionSort(arr, n);

    cout << "Después: ";
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;
}
