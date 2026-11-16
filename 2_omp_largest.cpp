// Compile: g++ -fopenmp 2_omp_largest.cpp -o largest
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

int main() {
    int n;
    cout << "Enter number of elements: ";
    cin >> n;
    vector<int> a(n);

    srand(time(0));
    for (int i = 0; i < n; i++) a[i] = rand() % 100000;
    if (n <= 20) {                       // print only small arrays
        cout << "Array: ";
        for (int i = 0; i < n; i++) cout << a[i] << " ";
        cout << endl;
    }

    // ---------- Sequential ----------
    double start = omp_get_wtime();
    int seq_max = a[0];
    for (int i = 0; i < n; i++)
        if (a[i] > seq_max) seq_max = a[i];
    double seq_time = omp_get_wtime() - start;

    // ---------- Parallel ----------
    start = omp_get_wtime();
    int par_max = a[0];
    #pragma omp parallel for reduction(max:par_max)
    for (int i = 0; i < n; i++)
        if (a[i] > par_max) par_max = a[i];
    double par_time = omp_get_wtime() - start;

    cout << "Sequential largest = " << seq_max << endl;
    cout << "Parallel largest   = " << par_max << endl;
    cout << "\nSequential time: " << seq_time << " s" << endl;
    cout << "Parallel time:   " << par_time << " s" << endl;
    return 0;
}
