#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <random>
#include <numeric>

struct ThreadData {
    const std::vector<int>* arr;
    size_t start;
    size_t end;
    long long partial_sum;
};

void* thread_sum(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    long long sum = 0;
    for (size_t i = data->start; i < data->end; ++i) {
        sum += (*data->arr)[i];
    }
    data->partial_sum = sum;
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <N> <M>\n";
        return 1;
    }

    size_t N = std::stoull(argv[1]);
    size_t M = std::stoull(argv[2]);

    if (N < 1000000 || M == 0) {
        std::cerr << "Error: N must be > 1,000,000 and M > 0.\n";
        return 1;
    }

    std::cout << "Generating...\n";
    std::vector<int> arr(N);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 100);
    for (auto& x : arr) x = dist(rng);

    std::cout << "Generation is complete\n";

    auto start_single = std::chrono::high_resolution_clock::now();
    long long sum_single = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        sum_single += arr[i];
    }
    auto end_single = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration_single = end_single - start_single;


    auto start_multi = std::chrono::high_resolution_clock::now();

    std::vector<pthread_t> threads(M);
    std::vector<ThreadData> thread_data(M);

    size_t block_size = N / M;
    for (size_t i = 0; i < M; ++i) {
        thread_data[i].arr = &arr;
        thread_data[i].start = i * block_size;
        thread_data[i].end = (i == M - 1) ? N : (i + 1) * block_size;
        thread_data[i].partial_sum = 0;

        pthread_create(&threads[i], nullptr, thread_sum, &thread_data[i]);
    }

    for (size_t i = 0; i < M; ++i) {
        pthread_join(threads[i], nullptr);
    }

    long long sum_multi = 0;
    for (size_t i = 0; i < M; ++i) {
        sum_multi += thread_data[i].partial_sum;
    }

    auto end_multi = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_multi = end_multi - start_multi;

    std::cout << "Time spent without threads: " << duration_single.count() << " seconds\n";
    std::cout << "Time spent with " << M << " threads: " << duration_multi.count() << " seconds\n";
    std::cout << "Sum check: " << (sum_single == sum_multi ? "OK" : "MISMATCH!") << "\n";

    return 0;
}
