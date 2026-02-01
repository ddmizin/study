#include <iostream>

// Variable templates

void print() {}

template <typename Head, typename... Tail> // пакет типов
void print(const Head& head, const Tail&... tail) { // распаковка пакета
    std::cout << head << "  /";
    std::cout << sizeof...(tail) << " elements" << '\n';
    print(tail...);
}

template <typename First, typename Second, typename... Types>
struct IsHomogeneous {
    static constexpr bool value = std::is_same_v<First, Second> && IsHomogeneous<Second, Types...>::value;
};

template <typename First, typename Second>
struct IsHomogeneous<First, Second> {
    static constexpr bool value = std::is_same_v<First, Second>;
};


int main() {
    print(1, 2.3, "abc");

    std::cout << IsHomogeneous<int, long long, double>::value;
}