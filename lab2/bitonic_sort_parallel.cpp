#include <bits/stdc++.h>
#include <pthread.h>
#include <ctime>
#include "generator.hpp"

// std::vector<int> arr {3, 7, 4, 8, 6, 2, 1, 5};
// int size = arr.size();

typedef struct {
    int low_edge; int count; int order; unsigned NUM_OF_THREADS; unsigned last_threads; std::vector<int> *arr;
} ThreadArg;

inline std::ostream& operator<<(std::ostream& os, std::vector<int> vec) {
    for (int i = 0; i < vec.size(); i++) {
        os << vec[i] << " "; 
    }
    return os;
}

void compAndSwap(std::vector<int>& arr, int i, int j, int order) {
    if (order == (arr[i] > arr[j])) {
        std::swap(arr[i], arr[j]);
    }
}

void *bitonic_merge(void *arg) {
    ThreadArg *args = (ThreadArg *)arg;
    int low_edge = args->low_edge; int count = args->count; int order = args->order;
    if (count > 1) {
        int k = count / 2;
        for (int i = low_edge; i < low_edge + k; i++) {
            compAndSwap(*args->arr, i, i + k, order);
        }
        if (args->last_threads > 1) {
            ThreadArg arg1 {low_edge, k, order, args->NUM_OF_THREADS, args->last_threads / 2, args->arr};
            ThreadArg arg2 {low_edge + k, k, order, args->NUM_OF_THREADS, args->last_threads / 2, args->arr};
            pthread_t thread1, thread2;
            pthread_create(&thread1, NULL, bitonic_merge, &arg1); pthread_create(&thread2, NULL, bitonic_merge, &arg2);
            pthread_join(thread1, NULL); pthread_join(thread2, NULL);
        } else {
            ThreadArg arg1 {low_edge, k, order, args->NUM_OF_THREADS, args->last_threads, args->arr};
            ThreadArg arg2 {low_edge + k, k, order, args->NUM_OF_THREADS, args->last_threads, args->arr};
            bitonic_merge(&arg1); bitonic_merge(&arg2);
       }
    }
    return nullptr;
}

void *bitonicSort(void *arg) {
    ThreadArg *threadarg = (ThreadArg *)arg;
    int low_edge = threadarg->low_edge; int count = threadarg->count; int order = threadarg->order;
    if (count > 1) {
        int k = count / 2;
        if (threadarg->NUM_OF_THREADS / 2) {
            ThreadArg arg1 {low_edge, k, 1, threadarg->NUM_OF_THREADS, threadarg->last_threads / 2, threadarg->arr};
            ThreadArg arg2 {low_edge + k, k, 0, threadarg->NUM_OF_THREADS, threadarg->last_threads / 2, threadarg->arr};
            pthread_t thread1, thread2;
            pthread_create(&thread1, NULL, bitonicSort, &arg1); pthread_create(&thread2, NULL, bitonicSort, &arg2);
            pthread_join(thread1, NULL); pthread_join(thread2, NULL);
            bitonic_merge(threadarg);
        } else {
            ThreadArg arg1 {low_edge, k, 1, threadarg->NUM_OF_THREADS, threadarg->last_threads, threadarg->arr};
            ThreadArg arg2 {low_edge + k, k, 0, threadarg->NUM_OF_THREADS, threadarg->last_threads, threadarg->arr};
            bitonicSort(&arg1); bitonicSort(&arg2);
            bitonic_merge(threadarg);
        }
    }
    return nullptr;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Invalid quantity of arguments!\n";
        return -1;
    }
    const unsigned NUM_OF_THREADS = std::stoi(argv[1]);
    bool isPowerOfTwo = NUM_OF_THREADS && !(NUM_OF_THREADS & (NUM_OF_THREADS - 1));
    if (!isPowerOfTwo) {
        std::cerr << "Quantity of threads is not a power of two!\n";
        return -1;
    }
    const int QUANTITY_OF_ELEMENTS = std::stoi(argv[2]);
    isPowerOfTwo = QUANTITY_OF_ELEMENTS && !(QUANTITY_OF_ELEMENTS & (QUANTITY_OF_ELEMENTS - 1));
    if (!isPowerOfTwo) {
        std::cerr << "Quantity of elements is not a power of two!\n";
        return -1;
    }
    if (NUM_OF_THREADS > QUANTITY_OF_ELEMENTS) {
        std::cerr << "Quantity of threads is greater than size of array!\n";
        return -1;
    }
    std::vector<int> vec = generate_tests(QUANTITY_OF_ELEMENTS);
    ThreadArg mainArg {0, QUANTITY_OF_ELEMENTS, 1, NUM_OF_THREADS, NUM_OF_THREADS, &vec};
    std::cout << "Initial array: " << vec << "\n";
    std::clock_t start = std::clock();
    bitonicSort(&mainArg);
    double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "Sorted array: " << vec << "\n";
    std::cout << "Sorted for " << std::fixed << duration << "sec\n";
}