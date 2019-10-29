#include "json.hpp"
#include <iostream>

int main() {
    std::string str1;
    std::ifstream in;
    std::string line;
    in.open(R"(D:\project\3sem_parserJSON\test.txt)");
    if (!in) {
        std::cout << "error opening file";
        return 0;
    }
    while (getline(in, line)) {
        str1 += line + '\n';
    }
    in.close();
    Json instance(str1);
    instance.parseFile(R"(D:\project\3sem_parserJSON\test.txt)");

//instance.print();
    return 0;
}