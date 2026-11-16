// Integral of x^2 from a to b using Trapezoidal Rule
// Compile: mpic++ 6_mpi_trapezoidal.cpp -o trap
// Run:     mpirun -np 4 ./trap
#include <iostream>
#include <mpi.h>
using namespace std;

double f(double x) { return x * x; }

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a = 0, b = 3;          // limits
    int n = 1024;                 // number of trapezoids (divisible by size)
    double h = (b - a) / n;

    int local_n = n / size;
    double local_a = a + rank * local_n * h;
    double local_b = local_a + local_n * h;

    double local = (f(local_a) + f(local_b)) / 2.0;
    for (int i = 1; i < local_n; i++)
        local += f(local_a + i * h);
    local *= h;

    double total;
    MPI_Reduce(&local, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
        cout << "Integral of x^2 from " << a << " to " << b << " = " << total << endl;

    MPI_Finalize();
    return 0;
}
