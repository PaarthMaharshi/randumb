// Compile: g++ -fopenmp 3_omp_primes.cpp -o primes
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

int main() {
    srand(time(0));
    int n = rand() % 100 + 2;      // random number from 2 to 101
    cout << "Primes between 2 and " << n << ":\n";

    #pragma omp parallel for
    for (int i = 2; i <= n; i++) {
        bool prime = true;
        for (int j = 2; j * j <= i; j++)
            if (i % j == 0) { prime = false; break; }
        if (prime) {
            #pragma omp critical
            cout << i << " ";
        }
    }
    cout << endl;
    return 0;
}
