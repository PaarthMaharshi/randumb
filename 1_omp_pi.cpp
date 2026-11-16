// Compile: g++ -fopenmp 1_omp_pi.cpp -o pi
#include <iostream>
#include <omp.h>
using namespace std;

int main() {
    long n = 100000000;
    double step = 1.0 / n;

    // ---------- Sequential ----------
    double start = omp_get_wtime();
    double sum = 0.0;
    for (long i = 0; i < n; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    double seq_pi = sum * step;
    double seq_time = omp_get_wtime() - start;

    // ---------- Parallel ----------
    start = omp_get_wtime();
    sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (long i = 0; i < n; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    double par_pi = sum * step;
    double par_time = omp_get_wtime() - start;

    cout << "Sequential Pi = " << seq_pi << endl;
    cout << "Parallel Pi   = " << par_pi << endl;
    cout << "\nSequential time: " << seq_time << " s" << endl;
    cout << "Parallel time:   " << par_time << " s" << endl;
    return 0;
}
