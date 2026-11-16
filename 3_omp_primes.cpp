// Compile: g++ -fopenmp 3_omp_primes.cpp -o primes
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

bool isPrime(int x) {
    for (int j = 2; j * j <= x; j++)
        if (x % j == 0) return false;
    return true;
}

int main() {
    srand(time(0));
    int n = rand() % 100 + 2;      // random number from 2 to 101
    cout << "Primes between 2 and " << n << ":\n";

    // ---------- Sequential ----------
    double start = omp_get_wtime();
    vector<int> seq_primes;
    for (int i = 2; i <= n; i++)
        if (isPrime(i)) seq_primes.push_back(i);
    double seq_time = omp_get_wtime() - start;

    // ---------- Parallel ----------
    start = omp_get_wtime();
    vector<int> par_primes;
    #pragma omp parallel for
    for (int i = 2; i <= n; i++) {
        if (isPrime(i)) {
            #pragma omp critical
            par_primes.push_back(i);
        }
    }
    double par_time = omp_get_wtime() - start;

    cout << "Sequential: ";
    for (int p : seq_primes) cout << p << " ";
    cout << "\nParallel:   ";
    for (int p : par_primes) cout << p << " ";
    cout << endl;

    cout << "\nSequential time: " << seq_time << " s" << endl;
    cout << "Parallel time:   " << par_time << " s" << endl;
    return 0;
}
