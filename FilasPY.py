f = 3
c = 3

twoDarr = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
]

print("RECORRIDO DE FILAS")

for i in range(f):
    print(f"Fila {i+1}: ", end="")
    for j in range(c):
        print(twoDarr[i][j], end=" ")
    print()
