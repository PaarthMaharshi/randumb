// Compile: g++ 7_mpi_odd_even_sort.cpp -o 7_mpi_odd_even_sort -I"$env:MSMPI_INC" -L"$env:MSMPI_LIB64" -lmsmpi
// Run:     mpiexec -n 4 .\7_mpi_odd_even_sort.exe

#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;                                           // numbers per process
    if (rank == 0) { cout << "Enter numbers per process:" << endl; cin >> n; }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    srand(time(0) + rank);                           // different randoms per process
    vector<int> local(n), other(n), all(n * size);
    for (int &x : local) x = rand() % 1000;

    MPI_Gather(local.data(), n, MPI_INT, all.data(), n, MPI_INT, 0, MPI_COMM_WORLD);
    vector<int> seq_arr = all;                       // copy for the sequential sort (used on rank 0)
    if (rank == 0) { cout << "Unsorted: "; for (int x : all) cout << x << " "; cout << endl; }

    // ---------- Parallel ----------
    double start = MPI_Wtime();
    sort(local.begin(), local.end());
    for (int ph = 0; ph < size; ph++) {
        int partner = (ph % 2 == rank % 2) ? rank + 1 : rank - 1;
        if (partner < 0 || partner >= size) continue;
        MPI_Sendrecv(local.data(), n, MPI_INT, partner, 0, other.data(), n, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        vector<int> t = local;
        t.insert(t.end(), other.begin(), other.end());
        sort(t.begin(), t.end());
        if (rank < partner) local.assign(t.begin(), t.begin() + n);   // keep small half
        else                local.assign(t.end() - n, t.end());       // keep big half
    }
    double par_time = MPI_Wtime() - start;

    MPI_Gather(local.data(), n, MPI_INT, all.data(), n, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        // ---------- Sequential odd-even transposition sort (rank 0 alone) ----------
        int N = seq_arr.size();
        start = MPI_Wtime();
        for (int ph = 0; ph < N; ph++)
            for (int i = ph % 2; i + 1 < N; i += 2)
                if (seq_arr[i] > seq_arr[i + 1]) swap(seq_arr[i], seq_arr[i + 1]);
        double seq_time = MPI_Wtime() - start;

        cout << "Parallel sorted:   "; for (int x : all) cout << x << " ";
        cout << "\nSequential sorted: "; for (int x : seq_arr) cout << x << " ";
        cout << "\n\nSequential time: " << seq_time << " s";
        cout << "\nParallel time:   " << par_time << " s" << endl;
    }
    MPI_Finalize();
}
