#include <bits/stdc++.h>
#include <pthread.h>
#include <ctime>

std::vector<int> arr {3, 7, 4, 8, 6, 2, 1, 5};
// std::vector<int> arr {3, 10, 7, 13, 11, 4, 16, 8, 12, 6, 2, 14, 15, 1, 5, 9};
int size = arr.size();

typedef struct {
    int low_edge; int count; int order;
} ThreadAttr;

inline std::ostream& operator<<(std::ostream& os, std::vector<int> vec) {
    for (int i = 0; i < vec.size(); i++) {
        os << vec[i] << " "; 
    }
    return os;
}

void compAndSwap(std::vector<int>& array, int i, int j, int order) {
    if (order == (array[i] > array[j])) {
        std::swap(array[i], array[j]);
    }
}

void bitonicMerge(std::vector<int>& array, int low_edge, int count, int order) {
    if (count > 1) {
        int k = count / 2;
        for (int i = low_edge; i < low_edge + k; i++) {
            compAndSwap(array, i, i + k, order);
        }
        bitonicMerge(array, low_edge, k, order);
        bitonicMerge(array, low_edge + k, k, order);
    }
}

void *bitonicSort(void *arg) {
    ThreadAttr *threadAttr = (ThreadAttr *)arg;
    int low_edge = threadAttr->low_edge; int length = threadAttr->count; int order = threadAttr->order;
    if (length > 1) {
        int k = length / 2;
        pthread_t thread1, thread2;
        ThreadAttr attr1 = {low_edge, k, 1};
        ThreadAttr attr2 = {low_edge + k, k, 0};
        // sort in ascending order since dir here is 1
        pthread_create(&thread1, NULL, bitonicSort, &attr1);
        // sort in descending order since dir here is 0
        pthread_create(&thread2, NULL, bitonicSort, &attr2);
        pthread_join(thread1, NULL); pthread_join(thread2, NULL);
        // Will merge whole sequence in ascending order
        // since dir=1.
        bitonicMerge(arr, low_edge, length, order);
    }
    pthread_exit(NULL);
}

int main() {
    std::clock_t start = std::clock();
    double duration;
    std::cout << "Initial array: " << arr << "\n";
    pthread_t mainThread;
    ThreadAttr mainAttr {0, size, 1};
    pthread_create(&mainThread, NULL, bitonicSort, &mainAttr);
    pthread_join(mainThread, NULL);
    std::cout << "Sorted array: " << arr << "\n";
    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << duration << "\n";
}