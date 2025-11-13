#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ordenamiento_sin_librerias.py
Script de consola, sin librerías externas.
Genera CSVs, resumen en consola y archivos de gráficas ASCII.
"""

import random, time, os, csv
from statistics import median

# -----------------------
# Configuración principal
# -----------------------
random.seed(42)
CARPETA_SALIDA = "resultados_ordenamiento"
os.makedirs(CARPETA_SALIDA, exist_ok=True)

TAMANOS = [100, 1000, 10000, 100000]
TIPOS = ["ordenado", "medianamente ordenado", "inverso"]
REPETICIONES = 3

# -----------------------
# Generadores de arreglos
# -----------------------
def generar_arreglo(tamano, tipo):
    if tipo == "ordenado":
        return list(range(tamano))
    if tipo == "inverso":
        return list(range(tamano-1, -1, -1))
    if tipo == "medianamente ordenado":
        a = list(range(tamano))
        swaps = max(1, tamano * 5 // 100)  # 5% swaps
        for _ in range(swaps):
            i = random.randrange(tamano)
            j = random.randrange(tamano)
            a[i], a[j] = a[j], a[i]
        return a
    # fallback aleatorio
    a = list(range(tamano))
    random.shuffle(a)
    return a

# -----------------------
# Métodos de ordenamiento (sin libs)
# -----------------------

def orden_insercion(arreglo):
    a = arreglo[:]
    for i in range(1, len(a)):
        clave = a[i]
        j = i - 1
        while j >= 0 and a[j] > clave:
            a[j+1] = a[j]
            j -= 1
        a[j+1] = clave
    return a

def orden_mezcla(arreglo):
    if len(arreglo) <= 1:
        return arreglo[:]
    m = len(arreglo) // 2
    izq = orden_mezcla(arreglo[:m])
    der = orden_mezcla(arreglo[m:])
    res = []
    i = j = 0
    while i < len(izq) and j < len(der):
        if izq[i] <= der[j]:
            res.append(izq[i]); i += 1
        else:
            res.append(der[j]); j += 1
    if i < len(izq): res.extend(izq[i:])
    if j < len(der): res.extend(der[j:])
    return res

def orden_quicksort_pivote(arreglo):
    # Quicksort in-place iterativo con pivote aleatorio (Lomuto)
    a = arreglo[:]
    n = len(a)
    if n <= 1:
        return a
    stack = [(0, n-1)]
    while stack:
        lo, hi = stack.pop()
        if lo >= hi:
            continue
        piv_idx = random.randint(lo, hi)
        a[piv_idx], a[hi] = a[hi], a[piv_idx]
        piv = a[hi]
        i = lo - 1
        for j in range(lo, hi):
            if a[j] <= piv:
                i += 1
                a[i], a[j] = a[j], a[i]
        a[i+1], a[hi] = a[hi], a[i+1]
        p = i + 1
        # empujar subrangos: empujar el mayor primero reduce pila
        left_size = p - 1 - lo
        right_size = hi - (p + 1)
        if left_size > right_size:
            if lo < p-1: stack.append((lo, p-1))
            if p+1 < hi: stack.append((p+1, hi))
        else:
            if p+1 < hi: stack.append((p+1, hi))
            if lo < p-1: stack.append((lo, p-1))
    return a

def orden_monticulo(arreglo):
    a = arreglo[:]
    n = len(a)
    def siftdown(start, end):
        root = start
        while True:
            child = 2*root + 1
            if child > end:
                break
            if child+1 <= end and a[child+1] < a[child]:
                child += 1
            if a[root] <= a[child]:
                break
            a[root], a[child] = a[child], a[root]
            root = child
    # heapify (min-heap)
    for start in range((n-2)//2, -1, -1):
        siftdown(start, n-1)
    res = []
    end = n - 1
    while end >= 0:
        a[0], a[end] = a[end], a[0]
        res.append(a[end])
        end -= 1
        if end >= 0:
            siftdown(0, end)
    return res

def orden_python_builtin(arreglo):
    return sorted(arreglo)

# -----------------------
# Registro de métodos
# -----------------------
METODOS = {
    "python": orden_python_builtin,
    "mezcla": orden_mezcla,
    "rapido_pivote": orden_quicksort_pivote,
    "monticulo": orden_monticulo,
    "insercion": orden_insercion
}

# -----------------------
# Ejecución de pruebas
# -----------------------
print("Iniciando pruebas de ordenamiento (sin librerías)...\n")
filas = []  # cada fila: dict con tamano,tipo,metodo,intento,tiempo,estado

for tam in TAMANOS:
    for tipo in TIPOS:
        for nombre, funcion in METODOS.items():
            # omitir inserción para tamaños grandes
            if nombre == "insercion" and tam > 1000:
                filas.append({"tamano": tam, "tipo": tipo, "metodo": nombre, "intento": None, "tiempo_seg": None, "estado": "omitido"})
                print(f"[omitido] inserción para tam={tam}, tipo={tipo}")
                continue
            tiempos = []
            correcto_total = True
            for intento in range(1, REPETICIONES+1):
                arr = generar_arreglo(tam, tipo)
                t0 = time.perf_counter()
                salida = funcion(arr)
                t1 = time.perf_counter()
                dt = t1 - t0
                tiempos.append(dt)
                estado = "ok" if salida == sorted(arr) else "error"
                if estado == "error":
                    correcto_total = False
                filas.append({"tamano": tam, "tipo": tipo, "metodo": nombre, "intento": intento, "tiempo_seg": dt, "estado": estado})
                print(f"tam={tam} tipo={tipo} metodo={nombre} intento={intento} tiempo={dt:.6f}s estado={estado}")
            med = median(tiempos)
            filas.append({"tamano": tam, "tipo": tipo, "metodo": nombre, "intento": "mediana", "tiempo_seg": med, "estado": ("ok" if correcto_total else "error")})
            print(f"  -> mediana {nombre} tam={tam} tipo={tipo}: {med:.6f}s  estado: {('ok' if correcto_total else 'error')}\n")

print("Pruebas finalizadas.\n")

# -----------------------
# Guardar CSVs
# -----------------------
archivo_raw = os.path.join(CARPETA_SALIDA, "resultados_crudos.csv")
archivo_medianas = os.path.join(CARPETA_SALIDA, "resumen_medianas.csv")
archivo_rank = os.path.join(CARPETA_SALIDA, "ranking_por_combinacion.csv")

campos_base = ["tamano", "tipo", "metodo", "intento", "tiempo_seg", "estado"]
with open(archivo_raw, "w", newline="", encoding="utf-8") as f:
    w = csv.DictWriter(f, fieldnames=campos_base)
    w.writeheader()
    for r in filas:
        w.writerow(r)

# extraer medianas
medianas = [r for r in filas if r["intento"] == "mediana"]
with open(archivo_medianas, "w", newline="", encoding="utf-8") as f:
    w = csv.DictWriter(f, fieldnames=["tamano", "tipo", "metodo", "tiempo_seg", "estado"])
    w.writeheader()
    for r in medianas:
        w.writerow({"tamano": r["tamano"], "tipo": r["tipo"], "metodo": r["metodo"], "tiempo_seg": r["tiempo_seg"], "estado": r["estado"]})

# ranking por (tamano,tipo)
ranking = []
for tam in TAMANOS:
    for tipo in TIPOS:
        grupo = [r for r in medianas if r["tamano"] == tam and r["tipo"] == tipo]
        grupo_ordenado = sorted(grupo, key=lambda x: (float('inf') if x["tiempo_seg"] is None else x["tiempo_seg"]))
        rank = 1
        for r in grupo_ordenado:
            ranking.append({"tamano": tam, "tipo": tipo, "metodo": r["metodo"], "tiempo_seg": r["tiempo_seg"], "estado": r["estado"], "rank": rank})
            rank += 1

with open(archivo_rank, "w", newline="", encoding="utf-8") as f:
    w = csv.DictWriter(f, fieldnames=["tamano", "tipo", "metodo", "tiempo_seg", "estado", "rank"])
    w.writeheader()
    for r in ranking:
        w.writerow(r)

print("Archivos CSV guardados en carpeta:", CARPETA_SALIDA)
print(" -", archivo_raw)
print(" -", archivo_medianas)
print(" -", archivo_rank, "\n")

# -----------------------
# Resumen por método
# -----------------------
# Para cada método calculamos: número de veces como 1er (victorias), promedio de rank, promedio de tiempo (solo donde no omitido)
met_info = {}
for metodo in METODOS.keys():
    met_info[metodo] = {"victorias": 0, "suma_ranks": 0, "cuenta_ranks": 0, "suma_tiempos": 0.0, "cuenta_tiempos": 0}

for item in ranking:
    metodo = item["metodo"]
    rank = item["rank"]
    if rank == 1:
        met_info[metodo]["victorias"] += 1
    met_info[metodo]["suma_ranks"] += rank
    met_info[metodo]["cuenta_ranks"] += 1
    if item["tiempo_seg"] is not None:
        met_info[metodo]["suma_tiempos"] += item["tiempo_seg"]
        met_info[metodo]["cuenta_tiempos"] += 1

print("Resumen por método:")
print("{:15} | {:7} | {:12} | {:12}".format("MÉTODO", "VICTORIAS", "PROM_RANK", "PROM_TIEMPO(s)"))
for m, info in met_info.items():
    prom_rank = (info["suma_ranks"] / info["cuenta_ranks"]) if info["cuenta_ranks"] else None
    prom_tiempo = (info["suma_tiempos"] / info["cuenta_tiempos"]) if info["cuenta_tiempos"] else None
    print("{:15} | {:7d} | {:12} | {:12}".format(
        m,
        info["victorias"],
        ("{:.2f}".format(prom_rank) if prom_rank is not None else "N/A"),
        ("{:.6f}".format(prom_tiempo) if prom_tiempo is not None else "N/A")
    ))
print()

# -----------------------
# Mostrar ranking completo por combinación en consola (top 3)
# -----------------------
print("Ranking TOP 3 por cada combinación (tamaño + tipo):\n")
for tam in TAMANOS:
    for tipo in TIPOS:
        grupo = [r for r in ranking if r["tamano"] == tam and r["tipo"] == tipo]
        grupo_sorted = sorted(grupo, key=lambda x: x["rank"])
        print(f"tamaño={tam} tipo={tipo}:")
        for r in grupo_sorted[:3]:
            ts = "omitido" if r["tiempo_seg"] is None else f"{r['tiempo_seg']:.6f}s"
            print(f"  rank {r['rank']:>1}: {r['metodo']:15} tiempo={ts} estado={r['estado']}")
        print()
print()

# -----------------------
# Crear gráficas ASCII (archivos) y mostrar resumen rápido en consola
# -----------------------
def generar_graficas_ascii(medianas, carpeta):
    archivos = []
    for tam in TAMANOS:
        ruta = os.path.join(carpeta, f"grafica_ascii_tam_{tam}.txt")
        archivos.append(ruta)
        with open(ruta, "w", encoding="utf-8") as f:
            f.write(f"Gráfica ASCII - tamaño {tam}\n")
            grupo = [r for r in medianas if r["tamano"] == tam]
            tiempos_validos = [r["tiempo_seg"] for r in grupo if r["tiempo_seg"] is not None]
            max_t = max(tiempos_validos) if tiempos_validos else 1.0
            # ordenar por método y luego tipo para consistencia
            for r in sorted(grupo, key=lambda x: (x["metodo"], x["tipo"])):
                if r["tiempo_seg"] is None:
                    barra = "(omitido)"
                    tstr = ""
                else:
                    largo = int((r["tiempo_seg"] / max_t) * 48)
                    if largo < 1: largo = 1
                    barra = "#" * largo
                    tstr = f" {r['tiempo_seg']:.6f}s"
                linea = f"{r['metodo']:15} | {r['tipo']:22} | {barra}{tstr}"
                f.write(linea + "\n")
    return archivos

archivos_ascii = generar_graficas_ascii(medianas, CARPETA_SALIDA)
print("Gráficas ASCII guardadas:")
for a in archivos_ascii:
    print(" -", a)
print()

# Mostrar una gráfica breve por tamaño en consola (solo primeros 6 líneas por claridad)
print("Ejemplo de gráfica ASCII para cada tamaño (primeras líneas):\n")
for ruta in archivos_ascii:
    print("Archivo:", ruta)
    with open(ruta, "r", encoding="utf-8") as f:
        for i, linea in enumerate(f):
            if i >= 8: break
            print(linea.rstrip())
    print("  ...\n")

print("Ejecución completa. Revisa la carpeta '{}' para CSVs y gráficas ASCII.".format(CARPETA_SALIDA))
print("CSV principales: resultados_crudos.csv, resumen_medianas.csv, ranking_por_combinacion.csv")
