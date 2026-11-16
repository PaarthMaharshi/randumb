// Compile: g++ -fopenmp 10_omp_matrix_mult.cpp -o matmul
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

int main() {
    int n;
    cout << "Enter size of matrix: ";
    cin >> n;
    vector<vector<int>> A(n, vector<int>(n)), B(n, vector<int>(n)),
                        S(n, vector<int>(n)), P(n, vector<int>(n));

    srand(time(0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }

    // ---------- Sequential ----------
    double start = omp_get_wtime();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            S[i][j] = 0;
            for (int k = 0; k < n; k++)
                S[i][j] += A[i][k] * B[k][j];
        }
    double seq_time = omp_get_wtime() - start;

    // ---------- Parallel ----------
    start = omp_get_wtime();
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            P[i][j] = 0;
            for (int k = 0; k < n; k++)
                P[i][j] += A[i][k] * B[k][j];
        }
    double par_time = omp_get_wtime() - start;

    if (n <= 5) {                        // print only small matrices
        cout << "Matrix A:\n";
        for (auto &r : A) { for (int x : r) cout << x << " "; cout << endl; }
        cout << "Matrix B:\n";
        for (auto &r : B) { for (int x : r) cout << x << " "; cout << endl; }
        cout << "Result:\n";
        for (auto &r : P) { for (int x : r) cout << x << " "; cout << endl; }
    }
    cout << "Sequential and parallel results " << (S == P ? "match" : "DO NOT match") << endl;

    cout << "\nSequential time: " << seq_time << " s" << endl;
    cout << "Parallel time:   " << par_time << " s" << endl;
    return 0;
}
