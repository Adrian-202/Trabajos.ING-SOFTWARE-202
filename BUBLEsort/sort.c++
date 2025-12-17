#include <iostream>
using namespace std;

void ordenar_vector(int vec[], int tam) {
    bool cambio = true;
    int contador = 0;
    while (cambio) {
        cambio = false;
        for (int j = 0; j < tam - 1 - contador; j++) {
            if (vec[j] > vec[j + 1]) {
                int temp = vec[j];
                vec[j] = vec[j + 1];
                vec[j + 1] = temp;
                cambio = true;
            }
        }
        contador++;
    }
}

int main() {
    int valores[] = {1, 4, 2, 3, 5, 8, 6};
    int n = sizeof(valores) / sizeof(valores[0]);
    ordenar_vector(valores, n);
    for (int i = 0; i < n; i++) cout << valores[i] << " ";
    return 0;
}