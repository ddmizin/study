#include <iostream>
#include <string>

//operator|(){}

struct Append {
    std::string str;

    Append(std::string str): str(str) {}
};

struct TwiceFlag {};
TwiceFlag twice() { return {}; }

struct StrBeforeTwice {
    std::string str;
};

std::string operator|(std::string lhs, std::string rhs) {
    return lhs + rhs;
}

StrBeforeTwice operator|(std::string lhs, TwiceFlag) {
    return { lhs };
}

std::string operator|(StrBeforeTwice twice, std::string str) {
    return twice.str + str + str;
}

std::string operator|(std::string lhs, const Append& a) {
    return lhs + a.str;
}





int main(){

    std::cout << (std::string("a") | Append("b") | twice() | std::string("c") | Append("d"));
    
    // abccd
    return 0;
}