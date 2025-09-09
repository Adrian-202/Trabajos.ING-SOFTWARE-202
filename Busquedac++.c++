#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> arr = {10, 15, 20, 30};
    int buscado = 20;
    auto it = find(arr.begin(), arr.end(), buscado);
    if(it != arr.end()){
        cout << buscado << " encontrado en índice " << distance(arr.begin(), it) << endl;
    } else {
        cout << buscado << " no encontrado" << endl;
    }
    return 0;
}
