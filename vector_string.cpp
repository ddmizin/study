#include <iostream>
#include <vector>
#include <string>

int main(){
    // -------------------------------std::vector
    std::vector<int> data1 = {1, 2, 3, 4, 5}; // вектор целых чисел типа int
    std::vector<std::string> data2; // вектор строк
    std::vector<std::string> v1;  // пустой вектор строк
    std::vector<std::string> v2(5);  // вектор из пяти пустых строк
    std::vector<std::string> v3(5, "hello");  // вектор из пяти строк "hello"
    std::vector<int> data = {1, 2, 3, 4, 5};
    int a = data[0];  // начальный элемент вектора
    int b = data[4];  // последний элемент вектора (в нём пять элементов)
    data[2] = -3;  // меняем элемент 3 на -3
    std::cout << data.size() << "\n"; // общее количество элементов в векторе
    std::cout << data[42] << "\n";  // неопределённое поведение: может произойти всё что угодно
    std::cout << data.at(0) << "\n";  // напечатается 1
    std::cout << data.at(42) << "\n";  // произойдёт исключение std::out_of_range — его можно будет перехватить и обработать
    std::cout << data.front() << "\n";  // то же, что data[0]
    std::cout << data.back() << "\n";  // то же, что data[data.size() - 1]
    if (!data.empty()) {
        // вектор не пуст, с ним можно работать
    }
    // --------------------------------------Добавление и удаление элементов
    data.push_back(6); // добавляет число в конец вектора
    data.pop_back();  // удаляет последний элемент вектора
    data.clear() // Удалить все элементы из вектора
    // --------------------------------------Многомерные векторы
    // создаём матрицу matrix из m строк, каждая из которых — вектор из n нулей
    std::vector<std::vector<int>> matrix(m, std::vector<int>(n));  
    for (size_t i = 0; i != m; ++i) {  
        for (size_t j = 0; j != n; ++j) {
            std::cin >> matrix[i][j];
        }
    }
    // напечатаем матрицу, выводя элементы через табуляцию
    for (size_t i = 0; i != m; ++i) {
        for (size_t j = 0; j != n; ++j) {
            std::cout << matrix[i][j] << "\t";
        }
        std::cout << "\n";
    }
    // std::sort (<algorithm>)
    std::sort(data.begin(), data.end()); // Сортировка вектора от начала до конца
    std::sort(data.rbegin(), data.rend()); // Сортировка по убыванию

    // --------------------------------------std::string
    std::string s = "Some string";
    s += ' ';  // добавляем отдельный символ в конец, это аналог push_back
    s += "functions";  // добавляем строку в конец
    std::string sub1 = s.substr(5, 6); // подстрока "string" из 6 символов начиная с 5-й позиции
    std::string sub2 = s.substr(12); // подстрока "functions" с 12-й позиции и до конца
    // ---------------------поиск символа или подстроки
    size_t pos1 = s.find(' ');  // позиция первого пробела, в данном случае 4
    size_t pos2 = s.find(' ', pos1 + 1);  // позиция следующего пробела (11)
    size_t pos3 = s.find("str");  // вернётся 5
    s.insert(5, "std::"); // вставка подстроки
    std::cout << s << "\n";  // Some std::string functions
    s.replace(0, 4, "Special"); // замена указанного диапазона на новую подстроку
    std::cout << s << "\n";  // Special std::string functions
    // удаление подстроки
    s.erase(8, 5);  // Special string functions
}