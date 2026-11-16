// Compile: g++ 4_mpi_pi.cpp -o 4_mpi_pi -I"$env:MSMPI_INC" -L"$env:MSMPI_LIB64" -lmsmpi
// Run:     mpiexec -n 4 .\4_mpi_pi.exe

#include <iostream>
#include <mpi.h>
using namespace std;

int main(int argc, char** argv){
    int rank, size;
    int n = 1000000;
    double step = 1.0 / n;
    double sum = 0.0;
    double total = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // ---------- Parallel ----------
    double start = MPI_Wtime();

    for(int i = rank; i < n; i+= size){
        double x = (i + 0.5) * step;
        sum += 4 / (1.0 + x * x);
    }

    MPI_Reduce(&sum, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if(rank == 0){
        double pi = total * step;
        double par_time = end - start;

        // ---------- Sequential (rank 0 alone) ----------
        start = MPI_Wtime();
        double seq_sum = 0.0;
        for(int i = 0; i < n; i++){
            double x = (i + 0.5) * step;
            seq_sum += 4 / (1.0 + x * x);
        }
        double seq_pi = seq_sum * step;
        double seq_time = MPI_Wtime() - start;

        cout << "parallel pi:   " << pi << endl;
        cout << "sequential pi: " << seq_pi << endl;
        cout << "\nsequential time: " << seq_time << " seconds" << endl;
        cout << "parallel time:   " << par_time << " seconds" << endl;
    }
    MPI_Finalize();
    return 0;
}
