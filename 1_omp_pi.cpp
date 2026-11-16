// Compile: g++ -fopenmp 1_omp_pi.cpp -o pi
#include <iostream>
#include <omp.h>
using namespace std;

int main() {
    long n = 1000000;
    double step = 1.0 / n, sum = 0.0;

    #pragma omp parallel for reduction(+:sum)
    for (long i = 0; i < n; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    cout << "Pi = " << sum * step << endl;
    return 0;
}
