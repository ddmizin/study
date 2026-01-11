#include <iostream>
#include <utility>

int Partition(int arr[], int left, int right){
    int pivot = arr[(left + right) / 2];
    int i = left - 1;
    int j = right + 1;
    while (true) {
        do { ++i; } while (arr[i] < pivot);
        do { --j; } while (arr[j] > pivot);
        if (i >= j) {
            return j;
        }
        std::swap(arr[i], arr[j]);
    }
}

void Quick_sort(int arr[], int left, int right){
    if (left < right){
        int p = Partition(arr, left, right);
        Quick_sort(arr, left, p);
        Quick_sort(arr, p + 1, right);
    }
}

int main(){
    int n;
    std::cin >> n;
    int arr[1000000];
    for (size_t i = 0; i < n; ++i){
        std::cin >> arr[i];
    }
    Quick_sort(arr, 0, n - 1);
    for (size_t i = 0; i < n; ++i){
        std::cout << arr[i] << " ";
    }
}