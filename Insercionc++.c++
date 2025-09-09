#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> arr = {10, 20, 30};
    arr.insert(arr.begin() + 1, 15);
    cout << "Después de insertar 15: ";
    for(int num : arr) cout << num << " ";
    cout << endl;
    return 0;
}
