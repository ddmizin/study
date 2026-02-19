// следующий код должен компилироваться

#include <assert.h>
#include <string>
#include <iostream>

class BracketBuilder {
    std::string str;
public:
    BracketBuilder() = default;

    BracketBuilder& operator[](const char*) {
        str += "[]";
        return *this;
    }

    BracketBuilder& operator()() {
        str += "()";
        return *this;
    }
    
    const char* as_string() {
        std::string copy = str;
        str.clear();
        return copy.c_str();
    }
};


int main() {
    BracketBuilder bb;
    
    assert(std::strcmp(bb[""]()[""]()()[""]().as_string(), "[]()[]()()[]()"));
    assert(std::strcmp(bb()[""]()[""]()()[""].as_string(), "()[]()[]()()[]"));
    std::cout << "All tests complited";
}