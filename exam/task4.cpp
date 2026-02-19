#include <iostream>
#include <string>

//operator|(){}

// 1. Структура для команды Append
struct Append {
    std::string data;
    Append(std::string s) : data(std::move(s)) {}
};

// 2. Маркер, который возвращает функция twice()
struct TwiceTag {};
TwiceTag twice() { return {}; }

// 3. Прокси-объект, который хранит текущую строку и ждёт следующую для удвоения
struct TwiceProxy {
    std::string current;
};

// --- Перегрузка операторов | ---

// string | Append("...") -> добавляем строку
std::string operator|(std::string lhs, const Append& rhs) {
    return lhs + rhs.data;
}

// string | twice() -> создаём прокси-объект
TwiceProxy operator|(std::string lhs, TwiceTag) {
    return { lhs };
}

// TwiceProxy | string -> удваиваем правую часть и добавляем к левой
std::string operator|(TwiceProxy proxy, const std::string& rhs) {
    return proxy.current + rhs + rhs;
}

// string | string -> обычная склейка (нужна для части | std::string("c"))
std::string operator|(std::string lhs, const std::string& rhs) {
    return lhs + rhs;
}


int main(){

    std::cout << (std::string("a") | Append("b") | twice() | std::string("c") | Append("d")); // abccd
    
    return 0;
}