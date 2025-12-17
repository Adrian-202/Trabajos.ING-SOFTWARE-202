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

enum class TipoNodo { Carpeta, Archivo };

class Nodo {
public:
    string id;
    string nombre;
    TipoNodo tipo;
    string contenido;
    vector<Nodo*> hijos;
    Nodo* padre;

    Nodo(string n, TipoNodo t, string c = "")
        : nombre(n), tipo(t), contenido(c), padre(nullptr) {
        id = to_string(rand());
    }

    ~Nodo() {
        for (Nodo* h : hijos) delete h;
    }

    json aJson() const {
        json j;
        j["id"] = id;
        j["nombre"] = nombre;
        j["tipo"] = (tipo == TipoNodo::Carpeta ? "carpeta" : "archivo");
        j["contenido"] = contenido;
        j["hijos"] = json::array();
        for (auto h : hijos) j["hijos"].push_back(h->aJson());
        return j;
    }

    static Nodo* desdeJson(const json& j) {
        TipoNodo tipo = j["tipo"] == "carpeta" ? TipoNodo::Carpeta : TipoNodo::Archivo;
        Nodo* n = new Nodo(j["nombre"], tipo, j.value("contenido", ""));
        n->id = j["id"];
        for (auto& h : j["hijos"]) {
            Nodo* hijo = desdeJson(h);
            hijo->padre = n;
            n->hijos.push_back(hijo);
        }
        return n;
    }
};

class NodoTrie {
public:
    map<char, NodoTrie*> hijos;
    bool fin = false;
};

class Trie {
    NodoTrie* raiz;
public:
    Trie() { raiz = new NodoTrie(); }

    void insertar(const string& palabra) {
        NodoTrie* act = raiz;
        for (char c : palabra) {
            if (!act->hijos[c]) act->hijos[c] = new NodoTrie();
            act = act->hijos[c];
        }
        act->fin = true;
    }
};

class ArbolJerarquia {
public:
    Nodo* raiz;
    Trie trie;

    ArbolJerarquia() {
        raiz = new Nodo("/", TipoNodo::Carpeta);
    }

    void crearNodo(const string& ruta, const string& nombre, TipoNodo tipo, const string& cont = "") {
        Nodo* n = new Nodo(nombre, tipo, cont);
        n->padre = raiz;
        raiz->hijos.push_back(n);
        trie.insertar(nombre);
    }

    void listar() {
        for (auto h : raiz->hijos)
            cout << h->nombre << endl;
    }

    void guardar() {
        ofstream f("arbol.json");
        f << setw(4) << raiz->aJson();
        f.close();
    }

    void cargar() {
        ifstream f("arbol.json");
        if (!f.is_open()) return;
        json j; f >> j;
        delete raiz;
        raiz = Nodo::desdeJson(j);
    }
};

ArbolJerarquia arbol;
vector<Nodo*> papelera;

void menu() {
    cout << "\nComandos:" << endl;
    cout << "mkdir, touch, ls, save, load, exit" << endl;
}

int main() {
    arbol.cargar();
    menu();

    string cmd;
    while (true) {
        cout << "> ";
        cin >> cmd;

        if (cmd == "mkdir") {
            string n; cin >> n;
            arbol.crearNodo("/", n, TipoNodo::Carpeta);
        }
        else if (cmd == "touch") {
            string n; cin >> n;
            arbol.crearNodo("/", n, TipoNodo::Archivo);
        }
        else if (cmd == "ls") {
            arbol.listar();
        }
        else if (cmd == "save") {
            arbol.guardar();
        }
        else if (cmd == "load") {
            arbol.cargar();
        }
        else if (cmd == "exit") {
            break;
        }
    }
    return 0;
}
