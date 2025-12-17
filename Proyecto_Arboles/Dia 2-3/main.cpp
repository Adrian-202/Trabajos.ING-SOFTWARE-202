#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum class TipoNodo { Carpeta, Archivo };

class Nodo {
public:
    string nombre;
    TipoNodo tipo;
    vector<Nodo*> hijos;
    Nodo* padre;

    Nodo(string n, TipoNodo t) : nombre(n), tipo(t), padre(nullptr) {}
};

class Arbol {
public:
    Nodo* raiz;

    Arbol() {
        raiz = new Nodo("/", TipoNodo::Carpeta);
    }

    void insertar(Nodo* padre, Nodo* hijo) {
        hijo->padre = padre;
        padre->hijos.push_back(hijo);
    }

    void preorden(Nodo* nodo) {
        if (!nodo) return;
        cout << nodo->nombre << endl;
        for (Nodo* h : nodo->hijos)
            preorden(h);
    }
};

int main() {
    Arbol arbol;
    Nodo* docs = new Nodo("docs", TipoNodo::Carpeta);
    arbol.insertar(arbol.raiz, docs);

    cout << "Dia 2-3: Arbol y recorrido preorden" << endl;
    arbol.preorden(arbol.raiz);
}
