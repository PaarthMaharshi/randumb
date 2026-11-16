// Compile: g++ 8_mpi_hyperquicksort.cpp -o 8_mpi_hyperquicksort -I"$env:MSMPI_INC" -L"$env:MSMPI_LIB64" -lmsmpi
// Run:     mpiexec -n 4 .\8_mpi_hyperquicksort.exe

#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;

// Sequential quicksort
void quicksort(vector<int> &a, int l, int r) {
    if (l >= r) return;
    int p = a[(l + r) / 2], i = l, j = r;
    while (i <= j) {
        while (a[i] < p) i++;
        while (a[j] > p) j--;
        if (i <= j) swap(a[i++], a[j--]);
    }
    quicksort(a, l, j);
    quicksort(a, i, r);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    if (rank == 0) { cout << "Enter numbers per process:" << endl; cin >> n; }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    srand(time(0) + rank);
    vector<int> local(n), all(n * size);
    for (int &x : local) x = rand() % 1000;

    MPI_Gather(local.data(), n, MPI_INT, all.data(), n, MPI_INT, 0, MPI_COMM_WORLD);
    vector<int> seq_arr = all;                       // copy for the sequential sort (used on rank 0)
    if (rank == 0) { cout << "Unsorted: "; for (int x : all) cout << x << " "; cout << endl; }

    // ---------- Parallel ----------
    double start = MPI_Wtime();
    sort(local.begin(), local.end());
    for (int bit = size / 2; bit >= 1; bit /= 2) {
        MPI_Comm g;
        MPI_Comm_split(MPI_COMM_WORLD, rank / (2 * bit), rank, &g);
        int pivot = local.empty() ? 0 : local[local.size() / 2];
        MPI_Bcast(&pivot, 1, MPI_INT, 0, g);
        MPI_Comm_free(&g);

        vector<int> low, high;
        for (int x : local) (x <= pivot ? low : high).push_back(x);

        int partner = rank ^ bit;
        bool upper = rank & bit;
        vector<int> send = upper ? low : high;
        local = upper ? high : low;

        int sn = send.size(), rn;
        MPI_Sendrecv(&sn, 1, MPI_INT, partner, 0, &rn, 1, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        vector<int> recv(rn);
        MPI_Sendrecv(send.data(), sn, MPI_INT, partner, 1, recv.data(), rn, MPI_INT, partner, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local.insert(local.end(), recv.begin(), recv.end());
        sort(local.begin(), local.end());
    }
    double par_time = MPI_Wtime() - start;

    int c = local.size();
    vector<int> counts(size), disp(size);
    MPI_Gather(&c, 1, MPI_INT, counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 1; i < size; i++) disp[i] = disp[i - 1] + counts[i - 1];
    MPI_Gatherv(local.data(), c, MPI_INT, all.data(), counts.data(), disp.data(), MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // ---------- Sequential quicksort (rank 0 alone) ----------
        start = MPI_Wtime();
        quicksort(seq_arr, 0, seq_arr.size() - 1);
        double seq_time = MPI_Wtime() - start;

        cout << "Parallel sorted:   "; for (int x : all) cout << x << " ";
        cout << "\nSequential sorted: "; for (int x : seq_arr) cout << x << " ";
        cout << "\n\nSequential time: " << seq_time << " s";
        cout << "\nParallel time:   " << par_time << " s" << endl;
    }
    MPI_Finalize();
}
