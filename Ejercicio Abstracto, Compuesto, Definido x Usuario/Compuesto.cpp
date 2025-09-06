#include <iostream>
using namespace std;


    int a;
    int b;
};

int main() {
    Num n;
    n.a = 5;
    n.b = 10;

    int suma = n.a + n.b;
    cout << "a: " << n.a << endl;
    cout << "b: " << n.b << endl;
    cout << "Suma: " << suma << endl;

    return 0;
}

