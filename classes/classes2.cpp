#include <iostream>

class String {
public:
    char* arr;
    size_t sz;
    size_t cap;

    String(size_t n): arr(new char[n + 1]), sz(n), cap(n + 1) { // delegating constructor
        arr[sz] = '\0';
    }
public:
    String() = default;
    String(int n, char c): String(n) {
        memset(arr, c, n);
    }
    String(std::initializer_list<char> list): String(list.size()) {
        std::copy(list.begin(), list.end(), arr);
    }
    String(const String& other): String(other.sz) {
        memcpy(arr, other.arr, sz);
    }

    String& operator=(String other) { // Copy-and-Swap idiom
        swap(other);
        return *this;
    }

    ~String() {
        delete[] arr;
    }

    void swap(String& other) {
        std::swap(arr, other.arr);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
    }
};


struct S {
    char arr[10];
    static int x;
    static const int y = 0;

    static void g() {
        std::cout << "Hi!" << '\n';
    }
    void f() const {
        std::cout << 1 << '\n';
    }
    void f() {
        std::cout << 2 << '\n';
    }

    char& operator[](size_t index){
        return arr[index];
    }
    const char& operator[](size_t index) const{
        return arr[index];
    }
};

int S::x = 1;


class Singleton {
private:
    static Singleton* ptr;

    Singleton() = default;
    Singleton(const Singleton& other) = delete;

    Singleton& operator=(const Singleton& other) = delete;
public:
    static Singleton& GetObject() {
        if (ptr == nullptr) {
            ptr = new Singleton();
        }
        return *ptr;
    }
};

Singleton* Singleton::ptr = nullptr;


struct Latitude {
    double value;
    explicit Latitude(double value): value(value) {}
    explicit operator double() const {
        return value;
    }
};

struct Longitude {
    double value;

    explicit Longitude(double value): value(value) {}
    explicit operator double() const{
        return value;
    }
};

Latitude operator""_lat(long double x){
    return Latitude(x);
}

Longitude operator""_log(long double x){
    return Longitude(x);
}


class BigInteger {};

BigInteger operator""_bi(unsigned long long x){
    return BigInteger();
}


std::ostream& operator<<(std::ostream& out, const String& str);
std::istream& operator>>(std::istream& in, const String& str);


int main() {
    String s; // default initialization
    String s2(5, 'r');
    String s3 = {'a', 'b', 'c', 'd'};
    String s4 = s3;
    s2 = s2;
    std::cout << s2.arr << '\n';

    S::g();
    S sf;
    const S& r = sf;
    sf.f();
    r.f();

    Singleton& slt = Singleton::GetObject();
    Latitude l(4.6);
    Latitude m = static_cast<Latitude>(4.7);
    double k = static_cast<double>(l);

    BigInteger bi = 1_bi;

    Latitude la = 4.7_lat;
    Longitude lo = 2.5_log;
}