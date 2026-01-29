#include <iostream>
#include <compare>

class Complex2 {
    double real, imag;
public:
    Complex2(double r, double i) : real(r), imag(i) {}
    
    // Автоматическая генерация всех операторов сравнения
    auto operator<=>(const Complex2& other) const = default;
};

int main() {
    Complex2 a(1.0, 2.0);
    Complex2 b(1.0, 2.0);
    Complex2 c(2.0, 1.0);
    
    std::cout << std::boolalpha;
    std::cout << "a == b: " << (a == b) << std::endl;  // true
    std::cout << "a != b: " << (a != b) << std::endl;  // false
    std::cout << "a < c: " << (a < c) << std::endl;    // зависит от порядка членов
    
    return 0;
}