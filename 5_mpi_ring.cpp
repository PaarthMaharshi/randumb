// Compile: g++ 5_mpi_ring.cpp -o 5_mpi_ring -I"$env:MSMPI_INC" -L"$env:MSMPI_LIB64" -lmsmpi
// Run:     mpiexec -n 4 .\5_mpi_ring.exe

#include <iostream>
#include <mpi.h>
using namespace std;

int main(int argc, char** argv){
    int rank, size, token;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int next = (rank + 1) % size;
    int prev = (rank - 1 + size) % size;

    if(rank == 0){
        cout << "enter token: "; cin >> token;
        int start_token = token;

        // ---------- Parallel (token passed around the ring) ----------
        double start = MPI_Wtime();
        MPI_Send(&token, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        MPI_Recv(&token, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        double par_time = MPI_Wtime() - start;

        // ---------- Sequential (one process visits every node in a loop) ----------
        start = MPI_Wtime();
        int seq_token = start_token;
        for(int i = 1; i < size; i++)
            seq_token += 1;
        double seq_time = MPI_Wtime() - start;

        cout << "rank 0 received token: " << token << endl;
        cout << "sequential token:      " << seq_token << endl;
        cout << "\nsequential time: " << seq_time << " seconds" << endl;
        cout << "parallel time:   " << par_time << " seconds" << endl;
    } else {
        MPI_Recv(&token, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        token += 1;
        MPI_Send(&token, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
