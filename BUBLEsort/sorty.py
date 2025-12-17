def algoritmo_burbuja(datos):
    limite = len(datos)
    intercambio = True
    pasada = 0
    while intercambio:
        intercambio = False
        for k in range(limite - 1 - pasada):
            if datos[k] > datos[k + 1]:
                temporal = datos[k]
                datos[k] = datos[k + 1]
                datos[k + 1] = temporal
                intercambio = True
        pasada += 1

elementos = [1, 4, 2, 3, 5, 8, 6]
print("Antes:", elementos)
algoritmo_burbuja(elementos)
print("Despues:", elementos)