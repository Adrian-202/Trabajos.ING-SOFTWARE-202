#include <iostream>
#include <map>
#include <vector>
#include <string>

using namespace std;

class Trie {
    map<char, Trie*> hijos;
    bool fin = false;

public:
    void insertar(const string& palabra, int i = 0) {
        if (i == palabra.size()) {
            fin = true;
            return;
        }
        if (!hijos[palabra[i]])
            hijos[palabra[i]] = new Trie();
        hijos[palabra[i]]->insertar(palabra, i + 1);
    }
};

int main() {
    Trie t;
    t.insertar("archivo");
    t.insertar("arbol");

    cout << "Dia 5-6: Trie implementado" << endl;
}
