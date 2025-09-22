def insertionSort(arr):
    for i in range(1, len(arr)):
        temp = arr[i]
        j = i - 1
        while j >= 0 and arr[j] > temp:
            arr[j + 1] = arr[j]
            j -= 1
        arr[j + 1] = temp

arr = [70, 15, 2, 51, 60, 80, 90, 1]
print("Antes:", arr)
insertionSort(arr)
print("Después:", arr)
