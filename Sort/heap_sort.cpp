#include <iostream>
#include <utility>

void sift_down(size_t i, int arr[], int n){
    while (true) {
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        int max = i;
        if (l < n && arr[l] > arr[max]) {
            max = l;
        }
        if (r < n && arr[r] > arr[max]) {
            max = r;
        }
        if (max == i) {
            break;
        }
        std::swap(arr[i], arr[max]);
        i = max;
    }
}

void build_heap(int arr[], int n){
    for (int i = n / 2 - 1; i >= 0; --i){
        sift_down(i, arr, n);
    }
}

void sort(int arr[], int n){
    for (int end = n - 1; end > 0; --end){
        std::swap(arr[0], arr[end]);
        --n;
        sift_down(0, arr, n);
    }
}

int main(){
    int n;
    std::cin >> n;
    int arr[n];
    for (size_t i = 0; i < n; ++i){
        std::cin >> arr[i];
    }

    build_heap(arr, n);
    sort(arr, n);
    for (size_t i = 0; i < n; ++i){
        std::cout << arr[i] << " ";
    }
}