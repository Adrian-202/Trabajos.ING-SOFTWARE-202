#include <iostream>
#include <vector>
#include <string>
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
        : nombre(n), tipo(t), contenido(c), padre(nullptr) {}

    json aJson() const {
        json j;
        j["nombre"] = nombre;
        j["tipo"] = (tipo == TipoNodo::Carpeta ? "carpeta" : "archivo");
        j["contenido"] = contenido;
        return j;
    }
};

int main() {
    cout << "Dia 1: Estructuras y formato JSON definidos." << endl;
    return 0;
}
