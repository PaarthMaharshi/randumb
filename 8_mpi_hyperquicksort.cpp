// Hyperquicksort. Number of processes must be a power of 2 (2, 4, 8...)
// Compile: mpic++ 8_mpi_hyperquicksort.cpp -o hqs
// Run:     mpirun -np 4 ./hqs
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Each process gets 5 random numbers and sorts them locally
    srand(time(0) + rank);
    vector<int> a(5);
    for (int &x : a) x = rand() % 100;
    sort(a.begin(), a.end());

    MPI_Comm comm = MPI_COMM_WORLD;
    for (int g = size; g > 1; g /= 2) {
        int r;
        MPI_Comm_rank(comm, &r);

        // Leader of the group broadcasts its median as pivot
        int pivot = a.empty() ? 0 : a[a.size() / 2];
        MPI_Bcast(&pivot, 1, MPI_INT, 0, comm);

        int half = g / 2;
        int partner = r ^ half;   // partner in the other half
        bool lower = r < half;

        // Lower half keeps <= pivot, upper half keeps > pivot
        vector<int> keep, send;
        for (int x : a)
            if ((x <= pivot) == lower) keep.push_back(x);
            else send.push_back(x);

        int sn = send.size(), rn;
        MPI_Sendrecv(&sn, 1, MPI_INT, partner, 0, &rn, 1, MPI_INT, partner, 0, comm, MPI_STATUS_IGNORE);
        vector<int> recv(rn);
        MPI_Sendrecv(send.data(), sn, MPI_INT, partner, 1, recv.data(), rn, MPI_INT, partner, 1,
                     comm, MPI_STATUS_IGNORE);

        // Merge kept + received (both already sorted)
        a.clear();
        merge(keep.begin(), keep.end(), recv.begin(), recv.end(), back_inserter(a));

        // Split group into two halves for next round
        MPI_Comm newcomm;
        MPI_Comm_split(comm, lower, r, &newcomm);
        if (comm != MPI_COMM_WORLD) MPI_Comm_free(&comm);
        comm = newcomm;
    }

    // Gather everything at process 0
    int n = a.size();
    vector<int> counts(size), disp(size);
    MPI_Gather(&n, 1, MPI_INT, counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    int total = 0;
    if (rank == 0)
        for (int i = 0; i < size; i++) { disp[i] = total; total += counts[i]; }
    vector<int> result(total);
    MPI_Gatherv(a.data(), n, MPI_INT, result.data(), counts.data(), disp.data(), MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Sorted: ";
        for (int x : result) cout << x << " ";
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}
