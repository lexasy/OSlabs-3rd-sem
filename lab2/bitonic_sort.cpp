#include <bits/stdc++.h>
#include <ctime>

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

void bitonicMerge(std::vector<int>& arr, int low_edge, int count, int order) {
    if (count > 1) {
        int k = count / 2;
        for (int i = low_edge; i < low_edge + k; i++) {
            compAndSwap(arr, i, i + k, order);
        }
        bitonicMerge(arr, low_edge, k, order);
        bitonicMerge(arr, low_edge + k, k, order);
    }
}

void bitonicSort(std::vector<int>& arr, int low_edge, int length, int order) {
    if (length > 1) {
        int k = length / 2;
        // sort in ascending order since dir here is 1
        bitonicSort(arr, low_edge, k, 1);
        // sort in descending order since dir here is 0
        bitonicSort(arr, low_edge + k, k, 0);
        // Will merge whole sequence in ascending order
        // since dir=1.
        bitonicMerge(arr, low_edge, length, order);
    }
}

void bsort(std::vector<int>& arr, int order) {
    bitonicSort(arr, 0, arr.size(), order);
}

int main() {
    std::vector<int> arr {3, 7, 4, 8, 6, 2, 1, 5};
    // std::vector<int> arr {3, 10, 7, 13, 11, 4, 16, 8, 12, 6, 2, 14, 15, 1, 5, 9};
    std::cout << "Initial array: " << arr << "\n";
    std::clock_t start = std::clock();
    bsort(arr, 1);
    double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "Sorted array: " << arr << "\n";
    std::cout << std::fixed << duration << "\n";
}
