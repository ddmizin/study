#include <iostream>

// Fold expressions (since c++17)

template <typename... Types>
struct AllPointers {
    static constexpr bool value = (std::is_pointer_v<Types> && ...);
};

template <typename Head, typename... Tail>
struct IsHomogeneous {
    static constexpr bool value = (std::is_same_v<Head, Tail> && ...);
};

template <typename... Types>
void print(const Types& ... types){
    (std::cout << ... << types);
}

int main() {
    print(4, 7.9, "asga");
}