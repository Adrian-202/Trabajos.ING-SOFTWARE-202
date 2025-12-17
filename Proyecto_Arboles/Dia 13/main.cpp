#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <random>
#include <functional>

// Biblioteca para JSON (asumo que se usa nlohmann/json)
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// ==============================================
// 1. ESTRUCTURAS BASE: TipoNodo y Nodo
// ==============================================

// Enumeración para el tipo de nodo: Carpeta o Archivo
enum class TipoNodo { Carpeta, Archivo };

/**
 * @brief Representa un nodo en la jerarquía de archivos (Carpeta o Archivo).
 * * Este nodo forma la base del árbol.
 */
class Nodo {
public:
    string id;
    string nombre;
    TipoNodo tipo;
    string contenido; // Solo relevante para archivos
    vector<Nodo*> hijos;
    Nodo* padre;

    // Constructor
    Nodo(string n, TipoNodo t, string c = "")
        : nombre(n), tipo(t), contenido(c), padre(nullptr) {

        // Generación de ID aleatorio basado en el nombre, tiempo y un generador Mersenne Twister
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::hash<std::string> hasheador;

        size_t valor_hash = hasheador(nombre) ^ hasheador(std::to_string(time(0))) ^ gen();
        id = std::to_string(valor_hash);
    }

    // Destructor (libera recursivamente la memoria de los hijos)
    ~Nodo() {
        for (Nodo* hijo : hijos) {
            delete hijo;
        }
    }

    // Convierte el nodo (y sus hijos) a un objeto JSON
    json aJson() const {
        json j;
        j["id"] = id;
        j["nombre"] = nombre;
        j["tipo"] = (tipo == TipoNodo::Carpeta ? "carpeta" : "archivo");
        j["contenido"] = contenido;
        json j_hijos = json::array();
        for (const auto& hijo : hijos) {
            j_hijos.push_back(hijo->aJson());
        }
        j["hijos"] = j_hijos;
        return j;
    }

    // Crea un nodo (y sus hijos) a partir de un objeto JSON
    static Nodo* desdeJson(const json& j) {
        TipoNodo tipo = (j["tipo"] == "carpeta" ? TipoNodo::Carpeta : TipoNodo::Archivo);
        Nodo* nodo = new Nodo(j["nombre"], tipo, j.value("contenido", ""));
        nodo->id = j["id"];

        if (j.contains("hijos")) {
            for (const auto& j_hijo : j["hijos"]) {
                Nodo* hijo = desdeJson(j_hijo);
                hijo->padre = nodo;
                nodo->hijos.push_back(hijo);
            }
        }
        return nodo;
    }
};

// ==============================================
// 2. ESTRUCTURA AUXILIAR: Trie para Autocompletado
// ==============================================

/**
 * @brief Nodo de la estructura de datos Trie (o Prefixtree).
 */
class NodoTrie {
public:
    map<char, NodoTrie*> hijos;
    bool esFinDePalabra;
    vector<string> nombres_completos; // Para manejar duplicados o nombres completos

    NodoTrie() : esFinDePalabra(false) {}
    ~NodoTrie() {
        for (auto const& [clave, valor] : hijos) delete valor;
    }
};

/**
 * @brief Implementación de la estructura Trie para búsqueda por prefijo (autocompletado).
 */
class Trie {
private:
    NodoTrie* raiz;

    // Función auxiliar recursiva para encontrar todas las palabras desde un nodo
    void encontrarTodasLasPalabras(NodoTrie* nodo, vector<string>& resultados) {
        if (nodo->esFinDePalabra) {
            resultados.insert(resultados.end(), nodo->nombres_completos.begin(), nodo->nombres_completos.end());
        }
        for (auto const& [clave, hijo] : nodo->hijos) {
            encontrarTodasLasPalabras(hijo, resultados);
        }
    }

public:
    Trie() { raiz = new NodoTrie(); }
    ~Trie() { delete raiz; }

    // Reinicia y reconstruye el Trie a partir del árbol de jerarquía
    void reiniciarYConstruir(Nodo* raiz_arbol) {
        delete raiz;
        raiz = new NodoTrie();
        asistenteConstruirTrie(raiz_arbol);
    }

    // Función auxiliar recursiva para construir el Trie
    void asistenteConstruirTrie(Nodo* nodo) {
        if (!nodo) return;
        // La raíz del sistema de archivos ("/") no se indexa para búsqueda
        if (nodo->nombre != "/") {
            insertarPalabra(nodo->nombre);
        }
        for (Nodo* hijo : nodo->hijos) {
            asistenteConstruirTrie(hijo);
        }
    }

    // Inserta una palabra (nombre de nodo) en el Trie
    void insertarPalabra(const string& palabra) {
        NodoTrie* actual = raiz;
        for (char c : palabra) {
            if (actual->hijos.find(c) == actual->hijos.end()) {
                actual->hijos[c] = new NodoTrie();
            }
            actual = actual->hijos[c];
        }
        actual->esFinDePalabra = true;

        // Agregar el nombre completo para manejar posibles duplicados de nombres
        if (std::find(actual->nombres_completos.begin(), actual->nombres_completos.end(), palabra) == actual->nombres_completos.end()) {
             actual->nombres_completos.push_back(palabra);
        }
    }

    // Realiza la búsqueda por prefijo y autocompleta
    vector<string> autocompletar(const string& prefijo) {
        NodoTrie* actual = raiz;
        vector<string> resultados;

        // Recorrer hasta el final del prefijo
        for (char c : prefijo) {
            if (actual->hijos.find(c) == actual->hijos.end()) {
                return resultados; // No hay coincidencias
            }
            actual = actual->hijos[c];
        }

        // Encontrar todas las palabras completas desde este punto
        encontrarTodasLasPalabras(actual, resultados);

        // Limpiar duplicados y ordenar los resultados
        std::sort(resultados.begin(), resultados.end());
        resultados.erase(std::unique(resultados.begin(), resultados.end()), resultados.end());

        return resultados;
    }
};

// ==============================================
// 3. ESTRUCTURA PRINCIPAL: ArbolJerarquia
// ==============================================

/**
 * @brief Clase principal que gestiona la estructura de árbol de jerarquía de archivos/carpetas.
 * * Incluye índices de búsqueda (Trie para prefijo, Map para exacto) para un acceso rápido.
 */
class ArbolJerarquia {
private:
    Nodo* raiz;
    Trie trie_nombres;
    map<string, Nodo*> mapa_busqueda_exacta; // Hash Map para búsqueda exacta por nombre

    // --- Funciones Auxiliares Privadas ---

    // Encuentra un nodo dado su ruta completa (ej: "/docs/reporte.txt")
    Nodo* encontrarNodoPorRuta(const string& ruta) {
        if (ruta == "/" || ruta.empty()) return raiz;

        string r = ruta;
        if (r.front() == '/') r.erase(0, 1); // Quitar '/' inicial
        if (r.empty()) return raiz;

        stringstream ss(r);
        string segmento;
        Nodo* actual = raiz;

        // Recorrer la ruta, segmento por segmento
        while (getline(ss, segmento, '/')) {
            if (segmento.empty()) continue;
            bool encontrado = false;
            for (Nodo* hijo : actual->hijos) {
                if (hijo->nombre == segmento) {
                    actual = hijo;
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado) return nullptr; // Segmento de ruta no existe
        }
        return actual;
    }

    // Función auxiliar para el recorrido en preorden
    void asistentePreorden(Nodo* nodo, vector<string>& resultado) {
        if (!nodo) return;
        string tipo_str = (nodo->tipo == TipoNodo::Carpeta ? "C" : "A");
        string ruta = mostrarRuta(nodo);
        resultado.push_back("[" + tipo_str + "] " + ruta);
        for (Nodo* hijo : nodo->hijos) {
            asistentePreorden(hijo, resultado);
        }
    }

    // Reconstrucción completa de los índices de búsqueda (usada tras load, rename o movimiento complejo)
    void reconstruirIndices() {
        mapa_busqueda_exacta.clear();
        trie_nombres.reiniciarYConstruir(raiz);

        // Función lambda recursiva para actualizar el mapa de hash
        function<void(Nodo*)> actualizarHash =
            [&](Nodo* nodo) {
            if (!nodo) return;
            if (nodo->nombre != "/") { // No indexar la raíz
                mapa_busqueda_exacta[nodo->nombre] = nodo;
            }
            for (Nodo* hijo : nodo->hijos) {
                actualizarHash(hijo);
            }
        };
        actualizarHash(raiz);
        cout << "Indices (Trie y Hash Map) reconstruidos." << endl;
    }

    // Remueve una entrada del Hash Map
    void removerEntradaHash(const string& nombre) {
        mapa_busqueda_exacta.erase(nombre);
    }

    // Inserta un nodo en el Hash Map
    void insertarEntradaHash(Nodo* nodo) {
        if (nodo->nombre != "/") {
            mapa_busqueda_exacta[nodo->nombre] = nodo;
        }
    }

public:
    // Constructor
    ArbolJerarquia() {
        raiz = new Nodo("/", TipoNodo::Carpeta);
        reconstruirIndices();
    }

    // Destructor
    ~ArbolJerarquia() {
        delete raiz;
    }

    // --- Operaciones CRUD y Persistencia ---

    /**
     * @brief Crea un nuevo nodo (Carpeta o Archivo) en la ruta padre especificada.
     */
    bool crearNodo(const string& ruta_padre, const string& nombre, TipoNodo tipo, const string& contenido = "") {
        Nodo* padre = encontrarNodoPorRuta(ruta_padre);
        if (!padre || padre->tipo != TipoNodo::Carpeta) {
            cerr << "Error: Ruta padre '" << ruta_padre << "' no encontrada o no es una carpeta." << endl;
            return false;
        }

        // Verificar si ya existe un nodo con ese nombre en el padre
        for (Nodo* hijo : padre->hijos) {
            if (hijo->nombre == nombre) {
                cerr << "Error: Ya existe un nodo con el nombre '" << nombre << "' en esta ruta." << endl;
                return false;
            }
        }

        Nodo* nuevoNodo = new Nodo(nombre, tipo, contenido);
        nuevoNodo->padre = padre;
        padre->hijos.push_back(nuevoNodo);

        // Actualizar índices
        trie_nombres.insertarPalabra(nuevoNodo->nombre);
        insertarEntradaHash(nuevoNodo);

        cout << (tipo == TipoNodo::Carpeta ? "Carpeta" : "Archivo") << " '" << nombre << "' creado en " << ruta_padre << endl;
        return true;
    }

    /**
     * @brief Renombra un nodo.
     */
    bool renombrarNodo(const string& ruta, const string& nuevo_nombre) {
        Nodo* nodo = encontrarNodoPorRuta(ruta);
        if (!nodo || nodo == raiz) {
            cerr << "Error: Nodo no encontrado o es la raiz ('/')." << endl;
            return false;
        }

        string nombre_anterior = nodo->nombre;

        // Verificar si un hermano ya tiene el nuevo nombre
        for (Nodo* hermano : nodo->padre->hijos) {
            if (hermano != nodo && hermano->nombre == nuevo_nombre) {
                cerr << "Error: Ya existe un nodo con el nombre '" << nuevo_nombre << "' en este directorio." << endl;
                return false;
            }
        }

        nodo->nombre = nuevo_nombre;
        // Se requiere reconstrucción completa de índices por el cambio de nombre
        reconstruirIndices();

        cout << "Nodo '" << nombre_anterior << "' renombrado a '" << nuevo_nombre << "'." << endl;
        return true;
    }

    /**
     * @brief Elimina un nodo (lo mueve a una 'papelera' temporal en memoria).
     */
    bool eliminarNodo(const string& ruta, vector<Nodo*>& papelera) {
        Nodo* nodo = encontrarNodoPorRuta(ruta);
        if (!nodo || nodo == raiz || !nodo->padre) {
            cerr << "Error: Nodo no encontrado o es la raiz ('/')." << endl;
            return false;
        }

        Nodo* padre = nodo->padre;
        auto& hijos_padre = padre->hijos;

        // Eliminar el puntero del vector de hijos del padre
        auto it = std::remove_if(hijos_padre.begin(), hijos_padre.end(),
            [&](Nodo* n) { return n == nodo; });

        if (it != hijos_padre.end()) {
            hijos_padre.erase(it, hijos_padre.end());

            // Mover a la papelera (se transfiere la propiedad del puntero)
            papelera.push_back(nodo);
            nodo->padre = nullptr; // Desvincular del árbol

            // Reconstrucción completa de índices ya que un subárbol completo podría haberse eliminado lógicamente
            reconstruirIndices();

            cout << "Nodo '" << nodo->nombre << "' movido a la papelera (puntero guardado)." << endl;
            return true;
        }
        return false;
    }

    /**
     * @brief Mueve un nodo de una ruta a otra.
     */
    bool moverNodo(const string& ruta_origen, const string& ruta_destino) {
        Nodo* nodo_origen = encontrarNodoPorRuta(ruta_origen);
        Nodo* padre_destino = encontrarNodoPorRuta(ruta_destino);

        if (!nodo_origen || nodo_origen == raiz) {
            cerr << "Error: Nodo de origen no encontrado o es la raiz." << endl;
            return false;
        }
        if (!padre_destino || padre_destino->tipo != TipoNodo::Carpeta) {
            cerr << "Error: Destino no encontrado o no es una carpeta." << endl;
            return false;
        }

        // Evitar mover un nodo a su propio subdirectorio
        Nodo* temp = padre_destino;
        while (temp) {
            if (temp == nodo_origen) {
                cerr << "Error: No se puede mover una carpeta a un subdirectorio propio." << endl;
                return false;
            }
            temp = temp->padre;
        }

        // 1. Eliminar de la lista de hijos del padre actual
        Nodo* padre_actual = nodo_origen->padre;
        auto& hijos_actuales = padre_actual->hijos;
        hijos_actuales.erase(
            std::remove(hijos_actuales.begin(), hijos_actuales.end(), nodo_origen),
            hijos_actuales.end()
        );

        // 2. Insertar en la lista de hijos del nuevo padre
        nodo_origen->padre = padre_destino;
        padre_destino->hijos.push_back(nodo_origen);

        // Reconstrucción completa de índices por si el movimiento alteró la unicidad de nombres
        reconstruirIndices();

        cout << "Nodo '" << nodo_origen->nombre << "' movido a " << ruta_destino << endl;
        return true;
    }

    /**
     * @brief Lista los hijos directos de un nodo (como el comando 'ls').
     */
    void listarHijos(const string& ruta) {
        Nodo* nodo = encontrarNodoPorRuta(ruta);
        if (!nodo || nodo->tipo != TipoNodo::Carpeta) {
            cerr << "Error: Ruta '" << ruta << "' no encontrada o no es una carpeta." << endl;
            return;
        }

        cout << "\nContenido de '" << ruta << "':" << endl;
        for (Nodo* hijo : nodo->hijos) {
            string tipo_str = (hijo->tipo == TipoNodo::Carpeta ? "DIR" : "FIL");
            cout << "[" << tipo_str << "] " << hijo->nombre << endl;
        }
        if (nodo->hijos.empty()) {
            cout << "(Vacio)" << endl;
        }
    }

    /**
     * @brief Muestra la ruta completa del nodo.
     */
    string mostrarRuta(Nodo* nodo) {
        if (!nodo) return "ERROR_NULO";
        if (nodo == raiz) return "/";

        string ruta = nodo->nombre;
        Nodo* actual = nodo->padre;
        while (actual && actual != raiz) {
            ruta = actual->nombre + "/" + ruta;
            actual = actual->padre;
        }
        return "/" + ruta;
    }

    /**
     * @brief Devuelve la lista de nodos en el recorrido Preorden.
     */
    vector<string> exportarPreorden() {
        vector<string> resultado;
        asistentePreorden(raiz, resultado);
        return resultado;
    }

    /**
     * @brief Guarda el árbol en un archivo JSON.
     */
    bool guardar(const string& nombre_archivo = "jerarquia.json") {
        try {
            json j = raiz->aJson();
            ofstream o(nombre_archivo);
            o << setw(4) << j << endl;
            o.close();
            cout << "Arbol guardado con exito en " << nombre_archivo << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error al guardar el JSON: " << e.what() << endl;
            return false;
        }
    }

    /**
     * @brief Carga el árbol desde un archivo JSON.
     */
    bool cargar(const string& nombre_archivo = "jerarquia.json") {
        try {
            ifstream i(nombre_archivo);
            if (!i.is_open()) {
                cerr << "Advertencia: Archivo " << nombre_archivo << " no encontrado. Iniciando con arbol raiz vacio." << endl;
                return false;
            }

            json j;
            i >> j;
            i.close();

            // Eliminar el árbol anterior antes de cargar el nuevo
            delete raiz;
            raiz = Nodo::desdeJson(j);

            // Reconstruir los índices de búsqueda
            reconstruirIndices();

            cout << "Arbol cargado con exito desde " << nombre_archivo << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error al cargar/parsear el JSON: " << e.what() << endl;
            // Si falla, inicializar un árbol vacío para evitar un estado inconsistente
            raiz = new Nodo("/", TipoNodo::Carpeta);
            reconstruirIndices();
            return false;
        }
    }

    // --- Métodos de Búsqueda Públicos ---

    /**
     * @brief Realiza autocompletado usando el Trie.
     */
    vector<string> buscarPorPrefijo(const string& prefijo) {
        return trie_nombres.autocompletar(prefijo);
    }

    /**
     * @brief Busca un nodo por nombre exacto usando el Hash Map.
     * * Nota: Esto puede devolver un nodo si hay nombres duplicados en diferentes rutas.
     */
    Nodo* buscarExacto(const string& nombre) {
        if (mapa_busqueda_exacta.count(nombre)) {
            return mapa_busqueda_exacta[nombre];
        }
        return nullptr;
    }
};

// ==============================================
// 4. INTERFAZ DE CONSOLA Y FUNCION PRINCIPAL
// ==============================================

ArbolJerarquia arbol;
vector<Nodo*> papelera; // Papelera de reciclaje temporal

void mostrarMenu() {
    cout << "\n" << string(50, '=') << endl;
    cout << "  MINI-SUITE DE GESTION DE ARCHIVOS (ARBOLES)" << endl;
    cout << string(50, '=') << endl;
    cout << "Comandos:" << endl;
    cout << "  - mkdir <ruta_padre> <nombre_carpeta>    (Crear Carpeta)" << endl;
    cout << "  - touch <ruta_padre> <nombre_archivo> [contenido] (Crear Archivo)" << endl;
    cout << "  - mv <ruta_origen> <ruta_destino>        (Mover Nodo)" << endl;
    cout << "  - rm <ruta>                              (Eliminar a Papelera)" << endl;
    cout << "  - ls <ruta>                              (Listar Hijos)" << endl;
    cout << "  - rename <ruta> <nuevo_nombre>           (Renombrar Nodo)" << endl;
    cout << "  - search <prefijo_o_nombre>              (Busqueda/Autocompletado: Trie y Hash)" << endl;
    cout << "  - export preorden                        (Exportar Recorrido)" << endl;
    cout << "  - save / load                            (Persistencia JSON)" << endl;
    cout << "  - papelera                               (Ver Contenido de Papelera)" << endl;
    cout << "  - clear_trash                            (Eliminar Papelera Permanentemente)" << endl;
    cout << "  - help / exit" << endl;
    cout << string(50, '=') << endl;
}

int main() {
    // Inicializar el generador de números pseudoaleatorios
    srand(time(0));

    // Intentar cargar el árbol al inicio
    arbol.cargar();
    mostrarMenu();

    string linea;
    while (true) {
        cout << "\n> ";
        // Leer la línea completa del comando
        if (!getline(cin, linea)) break;

        stringstream ss(linea);
        string comando, arg1, arg2, arg3;

        // Extraer el comando
        ss >> comando;

        if (comando == "exit") {
            cout << "Saliendo. No olvides hacer 'save'!" << endl;
            // Limpiar la papelera de reciclaje al salir (liberar memoria)
            for (Nodo* nodo_borrado : papelera) {
                delete nodo_borrado;
            }
            papelera.clear();
            break;
        } else if (comando == "help") {
            mostrarMenu();
        } else if (comando == "save") {
            arbol.guardar();
        } else if (comando == "load") {
            arbol.cargar();
        } else if (comando == "mkdir") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                arbol.crearNodo(arg1, arg2, TipoNodo::Carpeta);
            } else { cout << "Uso: mkdir <ruta_padre> <nombre_carpeta>" << endl; }
        } else if (comando == "touch") {
            ss >> arg1 >> arg2;
            string contenido;
            // Recoger todo el resto de la línea como contenido
            if (ss >> arg3) {
                contenido = arg3;
                string temp;
                while (ss >> temp) contenido += " " + temp;
            }
            if (!arg1.empty() && !arg2.empty()) {
                arbol.crearNodo(arg1, arg2, TipoNodo::Archivo, contenido);
            } else { cout << "Uso: touch <ruta_padre> <nombre_archivo> [contenido]" << endl; }
        } else if (comando == "ls") {
            ss >> arg1;
            arbol.listarHijos(arg1.empty() ? "/" : arg1);
        } else if (comando == "rename") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                arbol.renombrarNodo(arg1, arg2);
            } else { cout << "Uso: rename <ruta> <nuevo_nombre>" << endl; }
        } else if (comando == "rm") {
            ss >> arg1;
            if (!arg1.empty()) {
                 arbol.eliminarNodo(arg1, papelera);
            } else { cout << "Uso: rm <ruta>" << endl; }
        } else if (comando == "mv") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                arbol.moverNodo(arg1, arg2);
            } else { cout << "Uso: mv <ruta_origen> <ruta_destino>" << endl; }
        } else if (comando == "search") {
            ss >> arg1;
            if (!arg1.empty()) {
                Nodo* encontrado = arbol.buscarExacto(arg1);
                bool encontrado_hash = (encontrado != nullptr);

                if (encontrado_hash) {
                    cout << "\n[OK] Coincidencia exacta (Hash Map) con nombre '" << arg1 << "':" << endl;
                    cout << "  - Tipo: " << (encontrado->tipo == TipoNodo::Carpeta ? "Carpeta" : "Archivo") << endl;
                    cout << "  - Ruta: " << arbol.mostrarRuta(encontrado) << endl;
                }

                vector<string> resultados = arbol.buscarPorPrefijo(arg1);
                if (!resultados.empty()) {
                    cout << "\n[STAR] Autocompletado por prefijo ('" << arg1 << "'):" << endl;
                    for (const string& resultado : resultados) {
                        cout << "  - " << resultado << endl;
                    }
                } else if (!encontrado_hash) {
                    cout << "\n[FAIL] No se encontraron coincidencias." << endl;
                }
            } else { cout << "Uso: search <prefijo_o_nombre>" << endl; }
        } else if (comando == "export" && (ss >> arg1) && arg1 == "preorden") {
            vector<string> recorrido = arbol.exportarPreorden();
            cout << "\nRecorrido en Preorden:" << endl;
            // El recorrido en preorden visita la raíz, luego los hijos de izquierda a derecha.
            for (const string& s : recorrido) {
                cout << s << endl;
            }
        }

        // ==============================================
        // NUEVOS COMANDOS: Papelera
        // ==============================================
        else if (comando == "papelera") {
            if (papelera.empty()) {
                cout << "La papelera de reciclaje esta vacia." << endl;
            } else {
                cout << "\nContenido actual de la papelera (" << papelera.size() << " elementos):" << endl;
                for (size_t i = 0; i < papelera.size(); ++i) {
                    Nodo* nodo = papelera[i];
                    string tipo_str = (nodo->tipo == TipoNodo::Carpeta ? "DIR" : "FIL");
                    // Mostrar índice, tipo, nombre e ID
                    cout << "  [" << i << "] [" << tipo_str << "] " << nodo->nombre << " (ID: " << nodo->id << ")" << endl;
                }
                cout << "Estos nodos consumen memoria y no forman parte del arbol activo." << endl;
            }
        }

        else if (comando == "clear_trash") {
            if (papelera.empty()) {
                cout << "La papelera ya esta vacia. No hay nada que limpiar." << endl;
            } else {
                size_t num_eliminados = papelera.size();
                // 1. Liberar la memoria de cada nodo (liberando subárboles recursivamente)
                for (Nodo* nodo_borrado : papelera) {
                    delete nodo_borrado;
                }
                // 2. Vaciar el vector
                papelera.clear();
                cout << "Se han eliminado permanentemente " << num_eliminados << " elementos de la papelera." << endl;
            }
        }
        // ==============================================

        else {
            cout << "Comando no reconocido. Escribe 'help' para ver los comandos." << endl;
        }
    }

    return 0;
}
