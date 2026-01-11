#include <iostream>

void sort_insert(){
    int n;
    std::cin >> n;
    int data[100];
    for (size_t i = 0; i < n; ++i){
        std::cin >> data[i];
    }
    for (size_t i = 1; i < n; ++i){
        int key = data[i];
        int j = i - 1;
        while (j >= 0 && data[j] > key){
            data[j + 1] = data[j];
            --j;
        }
        data[j + 1] = key;
    }
    for (size_t i = 0; i < n; ++i){
        std::cout << data[i] << " ";
    }
}