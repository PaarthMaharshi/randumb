// Compile: g++ -fopenmp 2_omp_largest.cpp -o largest
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

int main() {
    int n;
    cout << "Enter number of elements: ";
    cin >> n;
    int a[n];

    srand(time(0));
    cout << "Array: ";
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 100;
        cout << a[i] << " ";
    }
    cout << endl;

    int mx = a[0];
    #pragma omp parallel for reduction(max:mx)
    for (int i = 0; i < n; i++)
        if (a[i] > mx) mx = a[i];

    cout << "Largest element = " << mx << endl;
    return 0;
}
