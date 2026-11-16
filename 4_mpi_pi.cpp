// Compile: mpic++ 4_mpi_pi.cpp -o mpi_pi
// Run:     mpirun -np 4 ./mpi_pi
#include <iostream>
#include <mpi.h>
using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long n = 1000000;
    double step = 1.0 / n, local = 0.0, pi;

    for (long i = rank; i < n; i += size) {      // each process takes every size-th strip
        double x = (i + 0.5) * step;
        local += 4.0 / (1.0 + x * x);
    }
    local *= step;

    MPI_Reduce(&local, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) cout << "Pi = " << pi << endl;

    MPI_Finalize();
    return 0;
}
