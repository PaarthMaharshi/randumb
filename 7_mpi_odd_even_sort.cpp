// One element per process. Compile: mpic++ 7_mpi_odd_even_sort.cpp -o oddeven
// Run: mpirun -np 6 ./oddeven
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(0) + rank);
    int val = rand() % 100, other;

    int arr[100];
    MPI_Gather(&val, 1, MPI_INT, arr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        cout << "Before: ";
        for (int i = 0; i < size; i++) cout << arr[i] << " ";
        cout << endl;
    }

    for (int phase = 0; phase < size; phase++) {
        int partner;
        if (phase % 2 == 0) {          // even phase: pairs (0,1),(2,3)...
            if (rank % 2 == 0)
                partner = rank + 1;
            else
                partner = rank - 1;
        } else {                       // odd phase: pairs (1,2),(3,4)...
            if (rank % 2 == 0)
                partner = rank - 1;
            else
                partner = rank + 1;
        }

        if (partner < 0 || partner >= size) continue;

        MPI_Sendrecv(&val, 1, MPI_INT, partner, 0, &other, 1, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (rank < partner) val = min(val, other);   // left keeps smaller
        else                val = max(val, other);   // right keeps larger
    }

    MPI_Gather(&val, 1, MPI_INT, arr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        cout << "After:  ";
        for (int i = 0; i < size; i++) cout << arr[i] << " ";
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}
