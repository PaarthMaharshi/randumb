// Compile: g++ -fopenmp 9_omp_producer_consumer.cpp -o pc
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

#define SIZE 5      // buffer size
#define N 10        // items to produce

int main() {
    int items[N];
    srand(time(0));
    for (int i = 0; i < N; i++) items[i] = rand() % 100;   // same items for both versions

    // ---------- Sequential ----------
    cout << "--- Sequential ---" << endl;
    int buffer[SIZE], count = 0, in = 0, out = 0;
    double start = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        buffer[in] = items[i];                 // produce
        in = (in + 1) % SIZE;
        count++;
        cout << "Produced: " << items[i] << endl;

        int item = buffer[out];                // consume
        out = (out + 1) % SIZE;
        count--;
        cout << "Consumed: " << item << endl;
    }
    double seq_time = omp_get_wtime() - start;

    // ---------- Parallel ----------
    cout << "--- Parallel ---" << endl;
    count = 0; in = 0; out = 0;
    start = omp_get_wtime();
    #pragma omp parallel sections num_threads(2)
    {
        #pragma omp section   // Producer
        for (int i = 0; i < N; i++) {
            bool done = false;
            while (!done) {
                #pragma omp critical
                if (count < SIZE) {
                    buffer[in] = items[i];
                    in = (in + 1) % SIZE;
                    count++;
                    cout << "Produced: " << items[i] << endl;
                    done = true;
                }
            }
        }

        #pragma omp section   // Consumer
        for (int i = 0; i < N; i++) {
            bool done = false;
            while (!done) {
                #pragma omp critical
                if (count > 0) {
                    int item = buffer[out];
                    out = (out + 1) % SIZE;
                    count--;
                    cout << "Consumed: " << item << endl;
                    done = true;
                }
            }
        }
    }
    double par_time = omp_get_wtime() - start;

    cout << "\nSequential time: " << seq_time << " s" << endl;
    cout << "Parallel time:   " << par_time << " s" << endl;
    return 0;
}
