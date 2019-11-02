// Copyright 2019 DM00n <teamvortex@yandex.ru>
#include "json.hpp"
#include <iostream>

int main() {
    std::string str1;
    std::ifstream in;
    std::string line;
    in.open(R"(C:\Users\Rex\Desktop\sdfsd\lab-01-parser\test.txt)");
    if (!in) {
        std::cout << "error opening file";
        return 0;
    }
    while (getline(in, line)) {
        str1 += line + '\n';
    }
    in.close();
    Json instance(str1);
    instance.parseFile(R"(C:\Users\Rex\Desktop\sdfsd\lab-01-parser\test.txt)");
    return 0;
}
