#include <iostream>
using namespace std;

int main() {
    int f = 3, c = 3;
    int twoDarr[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    cout << "RECORRIDO DE FILAS" << endl;

    for (int i = 0; i < f; i++) {
        cout << "Fila " << i+1 << ": ";
        for (int j = 0; j < c; j++) {
            cout << twoDarr[i][j] << " ";
        }
        cout << endl;
    }
}
