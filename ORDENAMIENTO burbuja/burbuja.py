def burbujaOrd(arr):
    n = len(arr)
    for i in range(n):
        for j in range(0, n - i - 1):
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]

arr = [70, 15, 2, 51, 60]
print("Antes:", arr)
burbujaOrd(arr)
print("Después:", arr)
