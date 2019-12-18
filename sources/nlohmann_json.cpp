// Copyright 2019 DM00n <teamvortex@yandex.ru>

#include <iostream>

using namespace std;

using json = nlohmann::json;

json magic(json obj) {
    json rez = json::array({});
    for (int i = 0; i < obj[0].size(); ++i) {
        json tmp = json::object({});
        for (int j = 0; j < obj.size(); ++j) {
            switch (j) {
                case 0:
                    tmp.push_back(json::object_t::
                                  value_type("ticker", obj[j][i]));
                    break;

                case 1:
                    tmp.push_back(json::object_t::
                                  value_type("id", obj[j][i]));
                    break;

                case 2:
                    tmp.push_back(json::object_t::
                                  value_type("desciption", obj[j][i]));
                    break;
                default:
                    break;
            }
        }
        rez += tmp;
    }
    return rez;
}

int main() {

    std::cout << "Hello, World!" << std::endl;

    json obj = json::array({
                                   {"Si-9.15", "RTS-9.15", "GAZP-9.15"},
                                   {100024, 100027, 100050},
                                   {"Futures contract for USD/RUB",
                                               "Futures contract for index RTS",
                                                           "Futures contract for Gazprom shares"}
                           });

    json test = magic(obj);
    cout << test;

    return 0;
}

