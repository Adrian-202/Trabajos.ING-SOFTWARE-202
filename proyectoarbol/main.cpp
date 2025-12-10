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


#include "json.hpp" 

using json = nlohmann::json;
using namespace std;

// ==============================================
// 1. ESTRUCTURAS BASE: TipoNodo y Nodo
// ==============================================

enum class TipoNodo { Carpeta, Archivo };

class Nodo {
public:
    string id;
    string nombre;
    TipoNodo tipo;
    string contenido; 
    vector<Nodo*> children;
    Nodo* parent; 

    Nodo(string n, TipoNodo t, string c = "") 
        : nombre(n), tipo(t), contenido(c), parent(nullptr) {
        
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::hash<std::string> hasher;
        
        size_t hash_val = hasher(nombre) ^ hasher(std::to_string(time(0))) ^ gen();
        id = std::to_string(hash_val); 
    }

    ~Nodo() {
        for (Nodo* child : children) {
            delete child;
        }
    }

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

    static Nodo* fromJson(const json& j) {
        TipoNodo tipo = (j["tipo"] == "carpeta" ? TipoNodo::Carpeta : TipoNodo::Archivo);
        Nodo* nodo = new Nodo(j["nombre"], tipo, j.value("contenido", ""));
        nodo->id = j["id"];
        
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
    vector<string> full_names; 

    TrieNode() : isEndOfWord(false) {}
    ~TrieNode() {
        for (auto const& [key, val] : children) delete val;
    }
};

class Trie {
private:
    TrieNode* root;

    void findAllWords(TrieNode* node, vector<string>& results) {
        if (node->isEndOfWord) {
            results.insert(results.end(), node->full_names.begin(), node->full_names.end());
        }
        for (auto const& [key, child] : node->children) {
            findAllWords(child, results);
        }
    }

public:
    Trie() { root = new TrieNode(); }
    ~Trie() { delete root; }

    void resetAndBuild(Nodo* arbol_root) {
        delete root;
        root = new TrieNode();
        buildTrieHelper(arbol_root);
    }
    
    void buildTrieHelper(Nodo* node) {
        if (!node) return;
        if (node->nombre != "/") { 
            insertWord(node->nombre);
        }
        for (Nodo* child : node->children) {
            buildTrieHelper(child);
        }
    }

    void insertWord(const string& word) {
        TrieNode* curr = root;
        for (char c : word) {
            if (curr->children.find(c) == curr->children.end()) {
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];
        }
        curr->isEndOfWord = true;
        
        if (std::find(curr->full_names.begin(), curr->full_names.end(), word) == curr->full_names.end()) {
             curr->full_names.push_back(word);
        }
    }

    vector<string> autocomplete(const string& prefix) {
        TrieNode* curr = root;
        vector<string> results;

        for (char c : prefix) {
            if (curr->children.find(c) == curr->children.end()) {
                return results; 
            }
            curr = curr->children[c];
        }
        
        findAllWords(curr, results);
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
    Trie name_trie; 
    map<string, Nodo*> hash_map_busqueda; 

    // --- Funciones Auxiliares Privadas ---
    
    Nodo* findNodeByPath(const string& path) {
        if (path == "/" || path.empty()) return root;

        string p = path;
        if (p.front() == '/') p.erase(0, 1);
        if (p.empty()) return root;

        stringstream ss(p);
        string segment;
        Nodo* current = root;

        while (getline(ss, segment, '/')) {
            if (segment.empty()) continue; 
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

    void preordenHelper(Nodo* node, vector<string>& result) {
        if (!node) return;
        string tipo_str = (node->tipo == TipoNodo::Carpeta ? "C" : "A");
        string path = mostrarRuta(node);
        result.push_back("[" + tipo_str + "] " + path);
        for (Nodo* child : node->children) {
            preordenHelper(child, result);
        }
    }
    
    // Reconstruccion completa de indices (usada tras load, rename o movimiento complejo)
    void rebuildIndices() {
        hash_map_busqueda.clear();
        name_trie.resetAndBuild(root); 

        function<void(Nodo*)> updateHash = 
            [&](Nodo* node) {
            if (!node) return;
            if (node->nombre != "/") { 
                hash_map_busqueda[node->nombre] = node; 
            }
            for (Nodo* child : node->children) {
                updateHash(child);
            }
        };
        updateHash(root);
        cout << "Indices (Trie y Hash Map) reconstruidos." << endl;
    }
    
    void removeHashEntry(const string& name) {
        hash_map_busqueda.erase(name);
    }
    
    void insertHashEntry(Nodo* node) {
        if (node->nombre != "/") {
            hash_map_busqueda[node->nombre] = node;
        }
    }

public:
    ArbolJerarquia() {
        root = new Nodo("/", TipoNodo::Carpeta); 
        rebuildIndices();
    }

    ~ArbolJerarquia() {
        delete root;
    }

    // --- Operaciones CRUD y Persistencia ---

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
        
        name_trie.insertWord(newNode->nombre);
        insertHashEntry(newNode);
        
        cout << (tipo == TipoNodo::Carpeta ? "Carpeta" : "Archivo") << " '" << nombre << "' creado en " << parent_path << endl;
        return true;
    }

    bool renombrarNodo(const string& path, const string& nuevo_nombre) {
        Nodo* node = findNodeByPath(path);
        if (!node || node == root) {
            cerr << "Error: Nodo no encontrado o es la raiz ('/')." << endl;
            return false;
        }
        
        string old_name = node->nombre;
        
        for (Nodo* sibling : node->parent->children) {
            if (sibling != node && sibling->nombre == nuevo_nombre) {
                cerr << "Error: Ya existe un nodo con el nombre '" << nuevo_nombre << "' en este directorio." << endl;
                return false;
            }
        }
        
        node->nombre = nuevo_nombre;
        rebuildIndices();
        
        cout << "Nodo '" << old_name << "' renombrado a '" << nuevo_nombre << "'." << endl;
        return true;
    }

    bool eliminarNodo(const string& path, vector<Nodo*>& papelera) {
        Nodo* node = findNodeByPath(path);
        if (!node || node == root || !node->parent) {
            cerr << "Error: Nodo no encontrado o es la raiz ('/')." << endl;
            return false;
        }

        Nodo* parent = node->parent;
        auto& children = parent->children;
        
        auto it = std::remove_if(children.begin(), children.end(), 
            [&](Nodo* n) { return n == node; });

        if (it != children.end()) {
            children.erase(it, children.end());
            papelera.push_back(node); 
            node->parent = nullptr; 
            
            rebuildIndices();
            
            cout << "Nodo '" << node->nombre << "' movido a la papelera (puntero guardado)." << endl;
            return true;
        }
        return false;
    }

    bool moverNodo(const string& src_path, const string& dest_path) {
        Nodo* src_node = findNodeByPath(src_path);
        Nodo* dest_parent = findNodeByPath(dest_path);

        if (!src_node || src_node == root) {
            cerr << "Error: Nodo de origen no encontrado o es la raiz." << endl;
            return false;
        }
        if (!dest_parent || dest_parent->tipo != TipoNodo::Carpeta) {
            cerr << "Error: Destino no encontrado o no es una carpeta." << endl;
            return false;
        }

        Nodo* temp = dest_parent;
        while (temp) {
            if (temp == src_node) {
                cerr << "Error: No se puede mover una carpeta a un subdirectorio propio." << endl;
                return false;
            }
            temp = temp->parent;
        }
        
        Nodo* current_parent = src_node->parent;
        auto& current_children = current_parent->children;
        current_children.erase(
            std::remove(current_children.begin(), current_children.end(), src_node),
            current_children.end()
        );

        src_node->parent = dest_parent;
        dest_parent->children.push_back(src_node);
        
        rebuildIndices();
        
        cout << "Nodo '" << src_node->nombre << "' movido a " << dest_path << endl;
        return true;
    }

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
            cout << "(Vacio)" << endl;
        }
    }

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

    vector<string> exportarPreorden() {
        vector<string> result;
        preordenHelper(root, result);
        return result;
    }

    bool guardar(const string& filename = "jerarquia.json") {
        try {
            json j = root->toJson();
            ofstream o(filename);
            o << setw(4) << j << endl;
            o.close();
            cout << "Arbol guardado con exito en " << filename << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error al guardar el JSON: " << e.what() << endl;
            return false;
        }
    }

    bool cargar(const string& filename = "jerarquia.json") {
        try {
            ifstream i(filename);
            if (!i.is_open()) {
                cerr << "Advertencia: Archivo " << filename << " no encontrado. Iniciando con arbol raiz vacio." << endl;
                return false; 
            }

            json j;
            i >> j;
            i.close();

            delete root; 
            root = Nodo::fromJson(j);
            
            rebuildIndices(); 
            
            cout << "Arbol cargado con exito desde " << filename << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error al cargar/parsear el JSON: " << e.what() << endl;
            root = new Nodo("/", TipoNodo::Carpeta);
            rebuildIndices();
            return false;
        }
    }

    // --- Metodos de Busqueda Publicos ---

    vector<string> buscarPorPrefijo(const string& prefix) {
        return name_trie.autocomplete(prefix);
    }

    Nodo* buscarExacto(const string& name) {
        if (hash_map_busqueda.count(name)) {
            return hash_map_busqueda[name];
        }
        return nullptr;
    }
};

// ==============================================
// 4. INTERFAZ DE CONSOLA Y FUNCION MAIN
// ==============================================

ArbolJerarquia arbol;
vector<Nodo*> papelera; 

void mostrarMenu() {
    cout << "\n" << string(50, '=') << endl;
    cout << "  MINI-SUITE DE GESTION DE ARCHIVOS (ARBOLES)" << endl;
    cout << string(50, '=') << endl;
    cout << "Comandos:" << endl;
    cout << "  - mkdir <ruta_padre> <nombre_carpeta>  (Crear Carpeta)" << endl;
    cout << "  - touch <ruta_padre> <nombre_archivo> [contenido] (Crear Archivo)" << endl;
    cout << "  - mv <ruta_origen> <ruta_destino>      (Mover Nodo)" << endl;
    cout << "  - rm <ruta>                           (Eliminar a Papelera)" << endl;
    cout << "  - ls <ruta>                           (Listar Hijos)" << endl;
    cout << "  - rename <ruta> <nuevo_nombre>        (Renombrar Nodo)" << endl;
    cout << "  - search <prefijo_o_nombre>           (Busqueda/Autocompletado)" << endl;
    cout << "  - export preorden                     (Exportar Recorrido)" << endl;
    cout << "  - save / load                         (Persistencia JSON)" << endl;
    cout << "  - help / exit" << endl;
    cout << string(50, '=') << endl;
}

int main() {
    srand(time(0)); 
    arbol.cargar(); 
    mostrarMenu();
    
    string line;
    while (true) {
        cout << "\n> ";
        if (!getline(cin, line)) break; 
        
        stringstream ss(line);
        string command, arg1, arg2, arg3;

        ss >> command;

        if (command == "exit") {
            cout << "Saliendo. No olvides hacer 'save'!" << endl;
            break;
        } else if (command == "help") {
            mostrarMenu();
        } else if (command == "save") {
            arbol.guardar();
        } else if (command == "load") {
            arbol.cargar();
        } else if (command == "mkdir") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                arbol.crearNodo(arg1, arg2, TipoNodo::Carpeta);
            } else { cout << "Uso: mkdir <ruta_padre> <nombre_carpeta>" << endl; }
        } else if (command == "touch") {
            ss >> arg1 >> arg2;
            string content;
            if (ss >> arg3) { 
                content = arg3;
                string temp;
                while (ss >> temp) content += " " + temp;
            }
            if (!arg1.empty() && !arg2.empty()) {
                arbol.crearNodo(arg1, arg2, TipoNodo::Archivo, content);
            } else { cout << "Uso: touch <ruta_padre> <nombre_archivo> [contenido]" << endl; }
        } else if (command == "ls") {
            ss >> arg1;
            arbol.listarHijos(arg1.empty() ? "/" : arg1);
        } else if (command == "rename") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                arbol.renombrarNodo(arg1, arg2);
            } else { cout << "Uso: rename <ruta> <nuevo_nombre>" << endl; }
        } else if (command == "rm") {
            ss >> arg1;
            if (!arg1.empty()) {
                 arbol.eliminarNodo(arg1, papelera);
            } else { cout << "Uso: rm <ruta>" << endl; }
        } else if (command == "mv") {
            ss >> arg1 >> arg2;
            if (!arg1.empty() && !arg2.empty()) {
                arbol.moverNodo(arg1, arg2);
            } else { cout << "Uso: mv <ruta_origen> <ruta_destino>" << endl; }
        } else if (command == "search") {
            ss >> arg1;
            if (!arg1.empty()) {
                Nodo* found = arbol.buscarExacto(arg1);
                bool hash_found = (found != nullptr);

                if (hash_found) {
                    cout << "\n[OK] Coincidencia exacta (Hash Map) con nombre '" << arg1 << "':" << endl;
                    cout << "  - Tipo: " << (found->tipo == TipoNodo::Carpeta ? "Carpeta" : "Archivo") << endl;
                    cout << "  - Ruta: " << arbol.mostrarRuta(found) << endl;
                }

                vector<string> results = arbol.buscarPorPrefijo(arg1);
                if (!results.empty()) {
                    cout << "\n[STAR] Autocompletado por prefijo ('" << arg1 << "'):" << endl;
                    for (const string& result : results) {
                        cout << "  - " << result << endl;
                    }
                } else if (!hash_found) {
                    cout << "\n[FAIL] No se encontraron coincidencias." << endl;
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
