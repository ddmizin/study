#include <iostream>

void Merge(int *arr, int left, int mid, int right){
    int i = left;
    int j = mid + 1;
    int k = left;
    int temp[1000000]; // temp[r - l]
    while ((i <= mid) && (j <= right)){
        if (arr[i] <= arr[j]){
            temp[k++] = arr[i++];
        }
        else {
            temp[k++] = arr[j++];
        }
    }
    while (i <= mid){
        temp[k++] = arr[i++];
    }
    while (j <= right){
        temp[k++] = arr[j++];
    }
    for (int idx = left; idx <= right; ++idx){
        arr[idx] = temp[idx];
    }
}

void Merge_sort(int *arr, int left, int right){
    if (left >= right){
        return;
    }
    int mid = left + (right - left) / 2;
    Merge_sort(arr, left, mid);
    Merge_sort(arr, mid + 1, right);
    Merge(arr, left, mid, right);
}

int main(){
    int n;
    std::cin >> n;
    int arr[1000000];
    for (size_t i = 0; i < n; ++i){
        std::cin >> arr[i];
    }
    Merge_sort(arr, 0, n - 1);
    for (size_t i = 0; i < n; ++i){
        std::cout << arr[i] << " ";
    }
}