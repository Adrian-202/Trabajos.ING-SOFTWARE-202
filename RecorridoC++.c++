#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> arr = {10, 20, 30};
    cout << "Recorrido:" << endl;
    for(int num : arr) cout << num << endl;
    return 0;
}
