#include <iostream>
#include <algorithm>

void sort_by_counting(){
    int n;
    std::cin >> n;
    int A[100];
    int B[100];
    int curr = 0;
    int k = -100000;
    for (size_t i = 0; i < n; ++i){
        std::cin >> curr;
        A[i] = curr;
        k = std::max(k, curr);
    }
    std::cout << '\n';
    int P[100] = {0};
    for (size_t i = 0; i < n; ++i){
        P[A[i]] += 1;
    }
    for (size_t i = 1; i <= k; ++i) {
        P[i] += P[i - 1];
    }
    for (size_t i = n; i > 0; --i){
        B[P[A[i - 1]] - 1] = A[i - 1];
        --P[A[i - 1]];
    }
    for (size_t i = 0; i < n; ++i){
        std::cout << B[i] << " ";
    }
}