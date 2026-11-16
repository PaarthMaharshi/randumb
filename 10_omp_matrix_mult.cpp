// Compile: g++ -fopenmp 10_omp_matrix_mult.cpp -o matmul
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

int main() {
    int n;
    cout << "Enter size of matrix: ";
    cin >> n;
    int A[n][n], B[n][n], C[n][n];

    srand(time(0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }

    cout << "Matrix A:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) cout << A[i][j] << " ";
        cout << endl;
    }
    cout << "Matrix B:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) cout << B[i][j] << " ";
        cout << endl;
    }

    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];
        }

    cout << "Result:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) cout << C[i][j] << " ";
        cout << endl;
    }
    return 0;
}
