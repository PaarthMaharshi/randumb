// Compile: g++ -fopenmp 9_omp_producer_consumer.cpp -o pc
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

#define SIZE 5      // buffer size
#define N 10        // items to produce

int main() {
    int buffer[SIZE], count = 0, in = 0, out = 0;
    srand(time(0));

    #pragma omp parallel sections num_threads(2)
    {
        #pragma omp section   // Producer
        for (int i = 1; i <= N; i++) {
            int item = rand() % 100;      // random item
            bool done = false;
            while (!done) {
                #pragma omp critical
                if (count < SIZE) {
                    buffer[in] = item;
                    in = (in + 1) % SIZE;
                    count++;
                    cout << "Produced: " << item << endl;
                    done = true;
                }
            }
        }

        #pragma omp section   // Consumer
        for (int i = 1; i <= N; i++) {
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
    return 0;
}
