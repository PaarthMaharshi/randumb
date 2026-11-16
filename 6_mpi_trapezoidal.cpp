// Compile: g++ 6_mpi_trapezoidal.cpp -o 6_mpi_trapezoidal -I"$env:MSMPI_INC" -L"$env:MSMPI_LIB64" -lmsmpi
// Run:     mpiexec -n 4 .\6_mpi_trapezoidal.exe

#include <iostream>
#include <mpi.h>
using namespace std;

double f(double x){
    return x * x;
}

int main(int argc, char** argv){
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a, b;
    int n;
    if(rank == 0){
        cout << "enter a b n: " ; cin >> a >> b >> n;
    }
    MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // ---------- Parallel ----------
    double start = MPI_Wtime();
    double h = (b - a) / n, sum = 0, total;
    for(int i = rank + 1; i < n; i += size){
        sum += f(a + i * h);
    }
    MPI_Reduce(&sum, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if(rank == 0){
        total = (total + (f(a) + f(b)) / 2) * h;
        double par_time = end - start;

        // ---------- Sequential (rank 0 alone) ----------
        start = MPI_Wtime();
        double seq = (f(a) + f(b)) / 2;
        for(int i = 1; i < n; i++)
            seq += f(a + i * h);
        seq *= h;
        double seq_time = MPI_Wtime() - start;

        cout << "parallel integral:   " << total << endl;
        cout << "sequential integral: " << seq << endl;
        cout << "\nsequential time: " << seq_time << " seconds" << endl;
        cout << "parallel time:   " << par_time << " seconds" << endl;
    }
    MPI_Finalize();
    return 0;
}
