#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int main() {
    json j;
    j["raiz"] = "sistema";

    ofstream archivo("arbol.json");
    archivo << j.dump(4);
    archivo.close();

    cout << "Dia 4: Persistencia JSON implementada" << endl;
    return 0;
}
