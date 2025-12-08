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


#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// ==============================================
// 1. ESTRUCTURAS BASE: TipoNodo y Nodo
// ==============================================

// Enumeración para el tipo de nodo
enum class TipoNodo { Carpeta, Archivo };

// Clase para el Nodo del Árbol
class Nodo {
public:
    string id;
    string nombre;
    TipoNodo tipo;
    string contenido; // Solo para Archivo
    vector<Nodo*> children;
    Nodo* parent; // Referencia al padre

    // Constructor
    Nodo(string n, TipoNodo t, string c = "")
        : nombre(n), tipo(t), contenido(c), parent(nullptr) {
        // Generación de un ID simple basado en nombre y tiempo/random
        id = n + to_string(time(0) + rand() % 10000);
    }

    // Destructor (libera recursivamente la memoria de los hijos)
    ~Nodo() {
        for (Nodo* child : children) {
            delete child;
        }
    }

    // Método para convertir el nodo a JSON
    json toJson() const {
        json j;
        j["id"] = id;
        j["nombre"] = nombre;
        j["tipo"] = (tipo == TipoNodo::Carpeta ? "carpeta" : "archivo");
        j["contenido"] = contenido;
        json j_children = json::array();
        for (const auto& child : children) {
            j_children.push_back(child->toJson());
        }
        j["children"] = j_children;
        return j;
    }

    // Método estático para crear un nodo desde JSON
    static Nodo* fromJson(const json& j) {
        TipoNodo tipo = (j["tipo"] == "carpeta" ? TipoNodo::Carpeta : TipoNodo::Archivo);
        Nodo* nodo = new Nodo(j["nombre"], tipo, j.value("contenido", ""));
        nodo->id = j["id"]; // Mantener el ID original

        if (j.contains("children")) {
            for (const auto& j_child : j["children"]) {
                Nodo* child = fromJson(j_child);
                child->parent = nodo;
                nodo->children.push_back(child);
            }
        }
        return nodo;
    }
};

// ==============================================
// 2. ESTRUCTURA AUXILIAR: Trie para Autocompletado
// ==============================================

class TrieNode {
public:
    map<char, TrieNode*> children;
    bool isEndOfWord;
    vector<string> full_names; // Lista de nombres de nodos que terminan aquí

    TrieNode() : isEndOfWord(false) {}

    ~TrieNode() {
        for (auto const& [key, val] : children) {
            delete val;
        }
    }
};

class Trie {
private:
    TrieNode* root;

    void insertWord(const string& word) {
        TrieNode* curr = root;
        for (char c : word) {
            if (curr->children.find(c) == curr->children.end()) {
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];
        }
        curr->isEndOfWord = true;
        // Solo agrega si no existe para evitar duplicados en el caso de la reconstrucción
        if (std::find(curr->full_names.begin(), curr->full_names.end(), word) == curr->full_names.end()) {
             curr->full_names.push_back(word);
        }
    }

    void findAllWords(TrieNode* node, vector<string>& results) {
        if (node->isEndOfWord) {
            results.insert(results.end(), node->full_names.begin(), node->full_names.end());
        }
        for (auto const& [key, child] : node->children) {
            findAllWords(child, results);
        }
    }

    void buildTrieHelper(Nodo* node) {
        if (!node) return;
        if (node->nombre != "/") { // No insertar el nombre de la raíz
            insertWord(node->nombre);
        }
        for (Nodo* child : node->children) {
            buildTrieHelper(child);
        }
    }

public:
    Trie() {
        root = new TrieNode();
    }

    ~Trie() {
        delete root;
    }

    // Reconstruye el Trie completo a partir del ArbolJerarquia
    void buildTrie(Nodo* arbol_root) {
        delete root;
        root = new TrieNode();
        buildTrieHelper(arbol_root);
        cout << "Trie reconstruido con éxito para autocompletado." << endl;
    }

    // Operación: Autocompletado por Prefijo
    vector<string> autocomplete(const string& prefix) {
        TrieNode* curr = root;
        vector<string> results;

        for (char c : prefix) {
            if (curr->children.find(c) == curr->children.end()) {
                return results;
            }
            curr = curr->children[c];
        }

        // Si el prefijo exacto es el nombre de un nodo, agrégalo.
        findAllWords(curr, results);

        // Eliminar duplicados si el prefijo es un nombre completo
        std::sort(results.begin(), results.end());
        results.erase(std::unique(results.begin(), results.end()), results.end());

        return results;
    }
};

// ==============================================
// 3. ESTRUCTURA PRINCIPAL: ArbolJerarquia
// ==============================================

class ArbolJerarquia {
private:
    Nodo* root;

    // Búsqueda interna de un nodo por ruta (ej. /carpeta1/archivo.txt)
    Nodo* findNodeByPath(const string& path) {
        if (path == "/" || path.empty()) return root;

        // Limpiar y dividir la ruta
        string p = path;
        if (p.front() == '/') p.erase(0, 1);
        if (p.empty()) return root;

        stringstream ss(p);
        string segment;
        Nodo* current = root;

        while (getline(ss, segment, '/')) {
            if (segment.empty()) continue; // Manejar // o / al final
            bool found = false;
            for (Nodo* child : current->children) {
                if (child->nombre == segment) {
                    current = child;
                    found = true;
                    break;
                }
            }
            if (!found) return nullptr;
        }
        return current;
    }

    // Recorrido Preorden (recursivo)
    void preordenHelper(Nodo* node, vector<string>& result) {
        if (!node) return;
        string tipo_str = (node->tipo == TipoNodo::Carpeta ? "C" : "A");
        string path = mostrarRuta(node);
        result.push_back("[" + tipo_str + "] " + path);
        for (Nodo* child : node->children) {
            preordenHelper(child, result);
        }
    }

public:
    ArbolJerarquia() {
        // El nodo raíz siempre es una carpeta
        root = new Nodo("/", TipoNodo::Carpeta);
    }

    ~ArbolJerarquia() {
        delete root;
    }

    // **Operación: Crear Nodo (mkdir/touch)**
    bool crearNodo(const string& parent_path, const string& nombre, TipoNodo tipo, const string& contenido = "") {
        Nodo* parent = findNodeByPath(parent_path);
        if (!parent || parent->tipo != TipoNodo::Carpeta) {
            cerr << "Error: Ruta padre '" << parent_path << "' no encontrada o no es una carpeta." << endl;
            return false;
        }

        for (Nodo* child : parent->children) {
            if (child->nombre == nombre) {
                cerr << "Error: Ya existe un nodo con el nombre '" << nombre << "' en esta ruta." << endl;
                return false;
            }
        }

        Nodo* newNode = new Nodo(nombre, tipo, contenido);
        newNode->parent = parent;
        parent->children.push_back(newNode);
        cout << (tipo == TipoNodo::Carpeta ? "Carpeta" : "Archivo") << " '" << nombre << "' creado en " << parent_path << endl;
        return true;
    }

    // **Operación: Renombrar Nodo**
    bool renombrarNodo(const string& path, const string& nuevo_nombre) {
        Nodo* node = findNodeByPath(path);
        if (!node || node == root) {
            cerr << "Error: Nodo no encontrado o es la raíz ('/')." << endl;
            return false;
        }
        if (!node->parent) { // No debería pasar si no es la raíz, pero por seguridad
             cerr << "Error: Nodo sin padre." << endl;
             return false;
        }

        for (Nodo* sibling : node->parent->children) {
            if (sibling != node && sibling->nombre == nuevo_nombre) {
                cerr << "Error: Ya existe un nodo con el nombre '" << nuevo_nombre << "' en este directorio." << endl;
                return false;
            }
        }

        string old_name = node->nombre;
        node->nombre = nuevo_nombre;
        cout << "Nodo '" << old_name << "' renombrado a '" << nuevo_nombre << "'." << endl;
        return true;
    }

    // **Operación: Eliminar Nodo (con papelera simulada)**
    bool eliminarNodo(const string& path, vector<Nodo*>& papelera) {
        Nodo* node = findNodeByPath(path);
        if (!node || node == root || !node->parent) {
            cerr << "Error: Nodo no encontrado o es la raíz ('/')." << endl;
            return false;
        }

        Nodo* parent = node->parent;
        auto& children = parent->children;

        // Usamos remove_if para encontrar el nodo a eliminar
        auto it = std::remove_if(children.begin(), children.end(),
            [&](Nodo* n) { return n == node; });

        if (it != children.end()) {
            children.erase(it, children.end());
            // El nodo se mueve a la papelera (se pierde su ruta, se guarda solo el puntero)
            papelera.push_back(node);
            node->parent = nullptr; // Desconectar
            cout << "Nodo '" << node->nombre << "' movido a la papelera (puntero guardado)." << endl;
            return true;
        }
        return false;
    }

    // **Operación: Mover Nodo (mv)**
    bool moverNodo(const string& src_path, const string& dest_path) {
        Nodo* src_node = findNodeByPath(src_path);
        Nodo* dest_parent = findNodeByPath(dest_path);

        if (!src_node || src_node == root) {
            cerr << "Error: Nodo de origen no encontrado o es la raíz." << endl;
            return false;
        }
        if (!dest_parent || dest_parent->tipo != TipoNodo::Carpeta) {
            cerr << "Error: Destino no encontrado o no es una carpeta." << endl;
            return false;
        }

        // Verificar que el destino no sea un hijo del origen (evitar bucle infinito)
        Nodo* temp = dest_parent;
        while (temp) {
            if (temp == src_node) {
                cerr << "Error: No se puede mover una carpeta a un subdirectorio propio." << endl;
                return false;
            }
            temp = temp->parent;
        }

        // 1. Eliminar del padre actual
        Nodo* current_parent = src_node->parent;
        auto& current_children = current_parent->children;
        current_children.erase(
            std::remove(current_children.begin(), current_children.end(), src_node),
            current_children.end()
        );

        // 2. Insertar en el nuevo padre
        src_node->parent = dest_parent;
        dest_parent->children.push_back(src_node);
        cout << "Nodo '" << src_node->nombre << "' movido a " << dest_path << endl;
        return true;
    }

    // **Operación: Listar Hijos (ls)**
    void listarHijos(const string& path) {
        Nodo* node = findNodeByPath(path);
        if (!node || node->tipo != TipoNodo::Carpeta) {
            cerr << "Error: Ruta '" << path << "' no encontrada o no es una carpeta." << endl;
            return;
        }

        cout << "\nContenido de '" << path << "':" << endl;
        for (Nodo* child : node->children) {
            string tipo_str = (child->tipo == TipoNodo::Carpeta ? "DIR" : "FIL");
            cout << "[" << tipo_str << "] " << child->nombre << endl;
        }
        if (node->children.empty()) {
            cout << "(Vacío)" << endl;
        }
    }

    // **Operación: Mostrar Ruta Completa**
    string mostrarRuta(Nodo* node) {
        if (!node) return "ERROR_NULO";
        if (node == root) return "/";

        string path = node->nombre;
        Nodo* current = node->parent;
        while (current && current != root) {
            path = current->nombre + "/" + path;
            current = current->parent;
        }
        return "/" + path;
    }

    // **Operación: Exportar Recorrido en Preorden**
    vector<string> exportarPreorden() {
        vector<string> result;
        preordenHelper(root, result);
        return result;
    }

    // **Operación: Persistencia (Guardar)**
    bool guardar(const string& filename = "jerarquia.json") {
        try {
            json j = root->toJson();
            ofstream o(filename);
            o << setw(4) << j << endl;
            o.close();
            cout << "Árbol guardado con éxito en " << filename << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error al guardar el JSON: " << e.what() << endl;
            return false;
        }
    }

    // **Operación: Persistencia (Cargar)**
    bool cargar(const string& filename = "jerarquia.json") {
        try {
            ifstream i(filename);
            if (!i.is_open()) {
                cerr << "Advertencia: Archivo " << filename << " no encontrado. Iniciando con árbol raíz vacío." << endl;
                return false;
            }

            json j;
            i >> j;
            i.close();

            // Limpiar el árbol actual
            delete root;

            root = Nodo::fromJson(j);
            cout << "Árbol cargado con éxito desde " << filename << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error al cargar/parsear el JSON: " << e.what() << endl;
            // Restablecer a un estado conocido si falla la carga
            root = new Nodo("/", TipoNodo::Carpeta);
            return false;
        }
    }

    // Método Getter para el Trie y el Hash Map
    Nodo* getRoot() const { return root; }
};

// ==============================================
// 4. INTERFAZ DE CONSOLA Y FUNCIÓN MAIN
// ==============================================

// Variables globales (para simplificar la interacción en el main)
ArbolJerarquia arbol;
Trie trie;
// Mapa Hash para búsqueda exacta rápida por nombre
map<string, Nodo*> hash_map_busqueda;
vector<Nodo*> papelera; // Papelera temporal

// Función para actualizar el mapa hash (recorre el árbol y mapea nombre a nodo)
void actualizarHashMap(Nodo* node) {
    if (!node) return;
    // La raíz (/) no se mapea típicamente por nombre
    if (node->nombre != "/") {
        // Sobreescribe si hay nombres duplicados, solo mantiene el último encontrado
        hash_map_busqueda[node->nombre] = node;
    }
    for (Nodo* child : node->children) {
        actualizarHashMap(child);
    }
}

// Inicializa las estructuras secundarias (Trie y Hash Map)
void inicializarIndices() {
    trie.buildTrie(arbol.getRoot());
    hash_map_busqueda.clear();
    actualizarHashMap(arbol.getRoot());
}

// Función para mostrar el menú
void mostrarMenu() {
    cout << "\n" << string(50, '=') << endl;
    cout << "  MINI-SUITE DE GESTIÓN DE ARCHIVOS (ÁRBOLES)" << endl;
    cout << string(50, '=') << endl;
    cout << "Comandos:" << endl;
    cout << "  - mkdir <ruta_padre> <nombre_carpeta>  (Crear Carpeta)" << endl;
    cout << "  - touch <ruta_padre> <nombre_archivo> [contenido] (Crear Archivo)" << endl;
    cout << "  - mv <ruta_origen> <ruta_destino>      (Mover Nodo)" << endl;
    cout << "  - rm <ruta>                           (Eliminar a Papelera)" << endl;
    cout << "  - ls <ruta>                           (Listar Hijos)" << endl;
    cout << "  - rename <ruta> <nuevo_nombre>        (Renombrar Nodo)" << endl;
    cout << "  - search <prefijo_o_nombre>           (Búsqueda/Autocompletado)" << endl;
    cout << "  - export preorden                     (Exportar Recorrido)" << endl;
    cout << "  - save                                (Guardar a JSON)" << endl;
    cout << "  - load                                (Cargar desde JSON)" << endl;
    cout << "  - help                                (Mostrar menú)" << endl;
    cout << "  - exit                                (Salir)" << endl;
    cout << string(50, '=') << endl;
}

// Función principal
int main() {
    // Inicialización del generador de números aleatorios para IDs
    srand(time(0));

    // 1. Cargar datos existentes o iniciar nuevo árbol
    arbol.cargar();

    // 2. Inicializar los índices (Trie y Hash Map)
    inicializarIndices();

    mostrarMenu();

    string line;
    while (true) {
        cout << "\n> ";
        // Leer la línea completa para manejar argumentos con espacios (como el contenido de touch)
        if (!getline(cin, line)) break;

        stringstream ss(line);
        string command, arg1, arg2, arg3;

        ss >> command;

        if (command == "exit") {
            cout << "Saliendo. ¡No olvides hacer 'save'!" << endl;
            break;
        } else if (command == "help") {
            mostrarMenu();
        } else if (command == "save") {
            arbol.guardar();
        } else if (command == "load") {
            if (arbol.cargar()) {
                inicializarIndices();
            }
        } else if (command == "mkdir") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                if (arbol.crearNodo(arg1, arg2, TipoNodo::Carpeta)) {
                    inicializarIndices();
                }
            } else { cout << "Uso: mkdir <ruta_padre> <nombre_carpeta>" << endl; }
        } else if (command == "touch") {
            ss >> arg1 >> arg2;
            string content;
            // Leer el resto de la línea como contenido (después de arg1 y arg2)
            if (ss >> arg3) {
                content = arg3;
                string temp;
                while (ss >> temp) content += " " + temp;
            }
            if (!arg1.empty() && !arg2.empty()) {
                if (arbol.crearNodo(arg1, arg2, TipoNodo::Archivo, content)) {
                    inicializarIndices();
                }
            } else { cout << "Uso: touch <ruta_padre> <nombre_archivo> [contenido]" << endl; }
        } else if (command == "ls") {
            ss >> arg1;
            arbol.listarHijos(arg1.empty() ? "/" : arg1);
        } else if (command == "rename") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                if (arbol.renombrarNodo(arg1, arg2)) {
                    inicializarIndices();
                }
            } else { cout << "Uso: rename <ruta> <nuevo_nombre>" << endl; }
        } else if (command == "rm") {
            ss >> arg1;
            if (!arg1.empty()) {
                 if (arbol.eliminarNodo(arg1, papelera)) {
                     inicializarIndices(); // El árbol cambió, reconstruir índices
                 }
            } else { cout << "Uso: rm <ruta>" << endl; }
        } else if (command == "mv") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                if (arbol.moverNodo(arg1, arg2)) {
                    inicializarIndices(); // El árbol cambió, reconstruir índices
                }
            } else { cout << "Uso: mv <ruta_origen> <ruta_destino>" << endl; }
        } else if (command == "search") {
            ss >> arg1;
            if (!arg1.empty()) {
                // Búsqueda exacta (Mapa Hash)
                bool hash_found = false;
                if (hash_map_busqueda.count(arg1)) {
                    Nodo* found = hash_map_busqueda[arg1];
                    cout << "\n✅ Coincidencia exacta (Hash Map) con nombre '" << arg1 << "':" << endl;
                    cout << "  - Tipo: " << (found->tipo == TipoNodo::Carpeta ? "Carpeta" : "Archivo") << endl;
                    cout << "  - Ruta: " << arbol.mostrarRuta(found) << endl;
                    hash_found = true;
                }

                // Autocompletado (Trie)
                vector<string> results = trie.autocomplete(arg1);
                if (!results.empty()) {
                    cout << "\n⭐ Autocompletado por prefijo ('" << arg1 << "'):" << endl;
                    for (const string& result : results) {
                        cout << "  - " << result << endl;
                    }
                } else if (!hash_found) {
                    cout << "\n❌ No se encontraron coincidencias." << endl;
                }
            } else { cout << "Uso: search <prefijo_o_nombre>" << endl; }
        } else if (command == "export" && (ss >> arg1) && arg1 == "preorden") {
            vector<string> recorrido = arbol.exportarPreorden();
            cout << "\nRecorrido en Preorden:" << endl;
            for (const string& s : recorrido) {
                cout << s << endl;
            }
        } else {
            cout << "Comando no reconocido. Escribe 'help' para ver los comandos." << endl;
        }
    }

    return 0;
}
