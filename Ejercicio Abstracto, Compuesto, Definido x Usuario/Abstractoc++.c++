#include <iostream>
using namespace std;

// Struct compuesto con números
struct Num {
    int a;
    int b;
};

int main() {
    Num n;   // objeto del struct
    n.a = 5; // primer número
    n.b = 10; // segundo número

    int suma = n.a + n.b; // suma de los dos números

    cout << "a: " << n.a << endl;
    cout << "b: " << n.b << endl;
    cout << "Suma: " << suma << endl;

    return 0;
}
