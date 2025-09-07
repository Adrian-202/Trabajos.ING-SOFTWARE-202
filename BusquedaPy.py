arr = [10, 15, 20, 30]
buscado = 20
if buscado in arr:
    print(f"{buscado} encontrado en el índice {arr.index(buscado)}")
else:
    print(f"{buscado} no encontrado")
