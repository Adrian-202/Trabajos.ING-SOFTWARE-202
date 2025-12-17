PROYECTO 1 – ARBOLES
Estructura de Datos

DESCRIPCION GENERAL
Este proyecto implementa una mini-suite de gestion de archivos en consola basada en una estructura de arbol general.
Permite crear, mover, renombrar, eliminar y buscar nodos (carpetas y archivos), exportar recorridos en preorden
y guardar/cargar la informacion mediante archivos JSON.

OBJETIVO
Desarrollar una aplicacion de consola que simule un sistema de archivos jerarquico, reforzando el uso de arboles,
recorridos, persistencia y estructuras auxiliares de busqueda.

ESTRUCTURA DEL PROYECTO

Proyecto_Arboles/
- Dia_07_09_Interfaz/main.cpp
- Dia_10_11_Pruebas/main.cpp
- json.hpp
- jerarquia.json
- README.txt

MODELO DE DATOS
Cada nodo del arbol contiene:
- id: identificador unico
- nombre: nombre del archivo o carpeta
- tipo: carpeta o archivo
- contenido: solo para archivos
- hijos: lista de nodos hijos
- padre: referencia al nodo padre

DESARROLLO POR DIAS

DIAS 7–9: INTERFAZ DE CONSOLA
En esta etapa se implementa la interfaz completa de consola, permitiendo la interaccion del usuario mediante
comandos similares a los de un sistema operativo.

FUNCIONALIDADES:
- Crear carpetas (mkdir)
- Crear archivos (touch)
- Mover nodos (mv)
- Eliminar nodos con papelera temporal (rm)
- Renombrar nodos (rename)
- Listar contenido (ls)
- Busqueda por nombre y prefijo (search)
- Exportar recorrido en preorden
- Guardar y cargar en JSON (save / load)

Se integran el arbol general, el Trie para autocompletado y la persistencia.

DIAS 10–11: PRUEBAS Y VALIDACIONES
En esta etapa se refuerza el proyecto con pruebas de integracion y manejo de errores.

VALIDACIONES:
- Rutas validas
- Prevencion de ciclos al mover nodos
- Evitar eliminar o renombrar la raiz
- Control de duplicados
- Manejo de errores de archivos JSON

CONSISTENCIA:
- Reconstruccion de indices tras eliminar, mover o renombrar nodos
- Sincronizacion entre arbol, Trie y Hash Map

BUSQUEDA
Se utilizan dos mecanismos:
- Trie para busqueda por prefijo
- Hash Map para busqueda exacta

PERSISTENCIA
El arbol se guarda en un archivo JSON local llamado jerarquia.json, conservando la estructura completa del sistema.

EJEMPLO DE USO
mkdir / docs
touch /docs reporte.txt Informe
ls /docs
search rep
export preorden
save

LENGUAJE Y HERRAMIENTAS
Lenguaje: C++
Libreria JSON: nlohmann/json
Entorno: Consola

CONCLUSION
El proyecto cumple con los requerimientos del cronograma, implementando correctamente las estructuras de datos
solicitadas y garantizando funcionalidad, consistencia y manejo de errores.
