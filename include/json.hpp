// Copyright 2018 Your Name <your_email>
#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

// Copyright 2018 Your Name <your_email>
#include <iostream>
#include <fstream>
#include <any>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <sstream>


using std::multimap;
using std::pair;
using std::map;
using std::string;
using std::vector;
using std::any;
using std::stringstream;
using std::fstream;
using std::any_cast;
using std::ifstream;

enum jsonEnums {
    Array,
    Object,
    Int,
    Float,
    String,
    Bool,
};

class Json {
public:
    // Конструктор из строки, содержащей Json-данные.
    explicit Json(const std::string &s) {
        fileContent = s;
    }

    Json() {}

    void deleteUseless() {
        bool filedFlag = false;
        bool filedBoolFlag = false;
        string s;
        for (uint64_t i = 0; i < fileContent.size(); i++) {
            char c = fileContent[i];
            if ((c == '"' || c == '{' || c == '}' || c == '['
                 || c == ']' || c == ',' || c == ':') &&
                filedFlag == false) {
                s.push_back(c);
                if (c == '"') {
                    filedFlag = true;
                }
                continue;
            }
            if (filedFlag == true) {
                s.push_back(c);
                if (c == '"') {
                    filedFlag = false;
                }
                continue;
            }
            if (c >= '0' && c <= '9') {
                s.push_back(c);
                continue;
            }
            if (c >= '.') {
                s.push_back(c);
                continue;
            }
            if (c == 'f' || c == 't') {
                s.push_back(c);
                filedBoolFlag = true;
                continue;
            }
            if (filedBoolFlag) {
                s.push_back(c);
                if (c == 'e') {
                    filedBoolFlag = false;
                }
            }
        }
        fileContent = s;
    }

public:
    // Метод возвращает true, если данный экземпляр
    // содержит в себе JSON-массив. Иначе false.
    bool is_array() const {
        for (uint64_t i = 0; i < fileContent.size(); i++) {
            if (fileContent[i] == '[') {
                return true;
            }
            if (fileContent[i] == '{') {
                return false;
            }
        }
        return false;
    }

    // Метод возвращает true, если данный экземпляр
    // содержит в себе JSON-объект. Иначе false.
    bool is_object() const {
        for (uint64_t i = 0; i < fileContent.size(); i++) {
            if (fileContent[i] == '{') {
                return true;
            }
            if (fileContent[i] == '[') {
                return false;
            }
        }
        return false;
    }

private:
    int getPair(uint64_t number, string object, int typeFlag) {
        if ((uint64_t) number >= object.size()) {
            return -1;
        }
        int cursPosition = number;

        if (typeFlag == Object) {
            cursPosition = getKey(number, object);
            if (cursPosition == -1) {
                return cursPosition;
            }
            cursPosition++;
        }

        cursPosition = getValue(cursPosition, object);
        return cursPosition;
    }

    int getKey(uint64_t number, string object) {
        bool flagStartReading = false;
        while ((uint64_t) number < object.size()) {
            char c = object[number];
            number++;
            if (c == '{') {
                return 0;
            }
            if ((c == '}' && (uint64_t) number == object.size())) {
                return -1;
            }
            if (c == '"' && !flagStartReading) {
                flagStartReading = true;
                continue;
            }
            if (c != '"' && flagStartReading) {
                key.push_back(c);
                continue;
            }
            if (c == '"' && flagStartReading) {
                return number;
            }
        }
        return -1;
    }

    int getValue(uint64_t number, string object) {
        switch (getType(number, object)) {
            case Object: {
                number = getObject(number, object);
                break;
            }
            case Array: {
                number = getArray(number, object);
                break;
            }
            case String: {
                number = getString(number, object);
                break;
            }
            case Int: {
                number = getInt(number, object);
                break;
            }
            case Float: {
                number = getFloat(number, object);
                break;
            }
            case Bool: {
                number = getBool(number, object);
                break;
            }
        }

        return number;
    }

    int getType(uint64_t number, string object) {
        int typeValue = -1;
        if ((uint64_t) number >= object.size()) {
            return typeValue;
        }
        if (key == "") {
            number = getKey(number, object) + 1;
            if (number == 0) {
                return -1;
            }
        }
        if (is_object(number, object)) {
            typeValue = Object;
            return typeValue;
        }
        if (is_array(number, object)) {
            typeValue = Array;
            return typeValue;
        }
        if (is_string(number, object)) {
            typeValue = String;
            return typeValue;
        }
        if (is_number(number, object)) {
            if (int_or_float(number, object)) {
                typeValue = Int;
                return typeValue;
            }
            typeValue = Float;
            return typeValue;
        }
        if (is_bool(number, object)) {
            typeValue = Bool;
            return typeValue;
        }
        return typeValue;
    }

    bool is_object(uint64_t number, string object) {
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c == '"' || c == '[' || c == ']' || c == ','
                || c == 'f' || c == 't' || (c >= '0' && c <= '9')) {
                return false;
            }
            if (c == '{') {
                return true;
            }
        }
        return false;
    }

    bool is_array(uint64_t number, string object) {
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c == '"' || c == '{' || c == '}' || c == ','
                || c == 'f' || c == 't' || (c >= '0' && c <= '9')) {
                return false;
            }
            if (c == '[') {
                return true;
            }
        }
        return false;
    }

    bool is_string(int number, string object) {
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c == '[' || c == '{' || c == '}' || c == ','
                || c == 'f' || c == 't' || (c >= '0' && c <= '9')) {
                return false;
            }
            if (c == '"') {
                return true;
            }
        }
        return false;
    }

    bool is_number(int number, string object) {
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c == '[' || c == '{' || c == '}' || c == ','
                || c == 'f' || c == 't' || c == '"') {
                return false;
            }
            if ((c >= '0' && c <= '9')) {
                return true;
            }
        }
        return false;
    }

    bool int_or_float(int number, string object) {
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c == '.') {
                return false;
            }
            if (c == ',' || c == '}') {
                return true;
            }
        }
        return true;
    }

    bool is_bool(int number, string object) {
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c == '[' || c == '{' || c == '}' || c == ','
                || (c >= '0' && c <= '9') || c == '"') {
                return false;
            }
            if (c == 'f' || c == 't') {
                return true;
            }
        }
        return false;
    }

    int getBool(int number, string object) {
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c == 't') {
                value = true;
                i += 5;


                return i;
            }
            if (c == 'f') {
                i += 6;

                value = false;
                return i;
            }
        }
        return -1;
    }

    int getFloat(int number, string object) {
        string s;
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c != ',' && c != ':' && c != ',' && c != ']' && c != '}') {
                s.push_back(c);
                continue;
            }
            if (c == ',' || c == ']' || c == '}') {
                stringstream ss;
                ss << s;
                float floatValue;
                ss >> floatValue;
                value = floatValue;
                i++;
                return i;
            }
        }
        return -1;
    }

    int getInt(int number, string object) {
        string s;
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c != ',' && c != ':' && c != ',' && c != ']' && c != '}') {
                s.push_back(c);
                continue;
            }
            if (c == ',' || c == ']' || c == '}') {
                stringstream ss;
                ss << s;
                int intValue;
                ss >> intValue;
                value = intValue;
                i++;
                return i;
            }
        }
        return -1;
    }

    int getString(int number, string object) {
        string s;
        bool skipIt = false;
        for (uint64_t i = (uint64_t) number; i <= object.size(); i++) {
            char c = object[i];
            if (c == '"' && !skipIt) {
                skipIt = true;
                continue;
            }
            if (c == '"' && skipIt) {
                skipIt = false;
                continue;
            }
            if (c != ',' && c != ':' && c != ',' && c != ']'
                && c != '}' && skipIt) {
                s.push_back(c);
                continue;
            }

            if ((c == ',' || c == ']' || c == '}') && !skipIt) {
                value = s;
                i++;
                return i;
            }
        }
        return -1;
    }

    int getArray(int number, string object) {
        vector <any> valueForMap;
        number++;
        while (object[number] != ']') {
            switch (getType(number, object)) {
                case Object: {
                    int numberForMap = number;

                    number = getObject(numberForMap, object);

                    multimap <string, any> mapForVector;

                    array.push_back(getMap(1, 0, mapForVector,
                                           any_cast<string>(value)));

                    mapForVector.clear();
                    break;
                }
                case Array: {
                    number = getArray(number + 1, object);
                    array.push_back(value);
                    valueForMap.push_back(value);
                    value.reset();
                    break;
                }
                case String: {
                    number = getString(number, object);
                    array.push_back(value);
                    valueForMap.push_back(value);
                    value.reset();
                    break;
                }
                case Int: {
                    number = getInt(number, object);
                    valueForMap.push_back(value);
                    array.push_back(value);
                    value.reset();
                    break;
                }
                case Float: {
                    number = getFloat(number, object);
                    valueForMap.push_back(value);
                    array.push_back(value);
                    value.reset();
                    break;
                }
                case Bool: {
                    number = getBool(number, object);
                    valueForMap.push_back(value);
                    array.push_back(value);
                    value.reset();
                    break;
                }
                case -1: {
                    value = valueForMap;
                    number++;
                    return number;
                }
            }
        }
        value = valueForMap;
        number++;
        return number;
    }

    int getObject(int number, string object) {
        string s;
        for (uint64_t i = (uint64_t) number; i < object.size(); i++) {
            char c = object[i];
            if (c != '}') {
                s.push_back(c);
            }
            if (c == '}') {
                s.push_back(c);
                value = s;
                return i + 1;
            }
        }
        return -1;
    }

    multimap <string, any> *getMap(int64_t number,
                                   int rang, multimap <string, any>
                                   &currentMap,
                                   string object) {
        key.clear();
        value.reset();
        if (getType(number, object) == Object) {
            key.clear();
            value.reset();
            number = getPair(number, object, Object);
            multimap <string, any> newMap;

            for (int i = 0; i < rang; i++) {
                std::cout << "     ";
            }
            std::cout << key << "{" << std::endl;
            rang++;
            string kek = any_cast<string>(value);
            currentMap.insert(
                    pair<string, any>(key, getMap(1, rang,
                                                  newMap, any_cast<string>(value))));
            rang--;
            for (int i = 0; i < rang; i++) {
                std::cout << "     ";
            }
            std::cout << "}" << std::endl;
            number++;

            key.clear();
            value.reset();
            getMap(number, rang, currentMap, object);
        } else {
            key.clear();
            value.reset();
            uint64_t numberForPrint = number;
            number = getPair(number, object, Object);
            if (number == -1) {
                return &currentMap;
            }
            currentMap.insert(pair<string, any>(key, value));
            printMe(numberForPrint, object, rang);
            getMap(number, rang, currentMap, object);
        }
        return &currentMap;
    }

    void printMe(int filedPosition, string object, int rang) {
        filedPosition = moveCursToField(filedPosition, object);
        for (int i = 0; i < rang; i++) {
            std::cout << "     ";
        }
        std::cout << key << " : ";
        keys.push_back(key);
        switch (getType(filedPosition, object)) {
            case Array: {
                std::cout << "(array)";
                const char *type =
                        any_cast < vector < any >> (value)[0].type().name();
                for (uint64_t j = 0;
                     j < any_cast < vector < any >> (value).size(); j++) {
                    if (std::string(type) == "i") {
                        std::cout <<
                                  any_cast<int>(any_cast < vector < any >>
                                                                        (value)[j]);
                        if (j < any_cast < vector < any >> (value).size() - 1) {
                            std::cout << ", ";
                        }
                        continue;
                    }
                    if (std::string(type) == "f") {
                        std::cout <<
                                  any_cast<float>(any_cast < vector < any >>
                                                                          (value)[j]);
                        if (j < any_cast < vector < any >> (value).size() - 1) {
                            std::cout << ", ";
                        }
                        continue;
                    } else {
                        std::cout <<
                                  any_cast<string>(any_cast < vector < any >>
                                                                           (value)[j]);
                        if (j < any_cast < vector < any >>
                                                        (value).size() - 1) {
                            std::cout << ", ";
                        }
                        continue;
                    }
                }
                break;
            }
            case String: {
                std::cout << "(string)" << any_cast<string>(value);
                values.push_back(any_cast<string>(value));
                break;
            }
            case Int: {
                std::cout << "(int)" << any_cast<int>(value);
                break;
            }
            case Float: {
                std::cout << "(float)" << any_cast<float>(value);
                break;
            }
            case Bool: {
                std::cout << "(bool)" << any_cast<bool>(value);
                break;
            }
        }
        std::cout << std::endl;
    }

    int moveCursToField(int number, string object) {
        char c = object[number];
        while (c != ':') {
            number++;
            c = object[number];
        }
        return number;
    }


public:
    // Метод возвращает значение по ключу key,
    // если экземпляр является JSON-объектом.
    // Значение может иметь один из следующих типов:
    // Json, std::string, double, bool или быть пустым.
    // Если экземпляр является JSON-массивом,
    // генерируется исключение.
    any &operator[](const string &_key) {
        any *kostil = getValue(_key);
        return *kostil;
    }

private:
    any *getValue(const string &_key) {
        for (auto itr = parsedObject.begin();
             itr != parsedObject.end(); ++itr) {
            if (itr->first == _key) {
                return &itr->second;
            }
        }
        return 0;
    }

public:
    // Метод возвращает значение по индексу index,
    // если экземпляр является JSON-массивом.
    // Значение может иметь один из следующих типов:
    // Json, std::string, double, bool или быть пустым.
    // Если экземпляр является JSON-объектом, генерируется исключение.
    any &operator[](int index) {
        return reinterpret_cast<any &>(parsedArray[index]);
    }

    // Метод возвращает объект класса Json из строки, содержащей Json-данные.
    Json parse(const std::string &s) {
        deleteUseless();
        std::cout << s << std::endl;
        if (is_array()) {
            std::cout << "array" << std::endl;
            getArray(0, fileContent);
            parsedArray = array;
        }
        if (is_object()) {
            std::cout << "object" << std::endl;
            multimap <string, any> kek;
            parsedObject = *getMap(1, 0, kek, fileContent);
        }
        return Json(fileContent);
    }

    // Метод возвращает объекта класса Json из файла,
    // содержащего Json-данные в текстовом формате.
    Json parseFile(const std::string &path_to_file) {
        ifstream jsonFile(path_to_file);
        if (!jsonFile.is_open()) {
            std::cout << "Opening file error!" << std::endl;
            return Json();
        }
        string kostil;
        while (getline(jsonFile, kostil)) {
            fileContent.append(kostil);
        }
        deleteUseless();
        std::cout << fileContent << std::endl;
        if (is_array()) {
            std::cout << "array" << std::endl;
            getArray(0, fileContent);
            parsedArray = array;
        }
        if (is_object()) {
            std::cout << "object" << std::endl;
            multimap <string, any> kek;
            parsedObject = *getMap(1, 0, kek, fileContent);
        }
        return Json(fileContent);
    }

public:
    string fileContent;
    string key;
    vector <string> keys;
    vector <any> values;
    any value;
    vector <any> array;

public:
    multimap <string, any> parsedObject;
    vector <any> parsedArray;
};

#endif // INCLUDE_HEADER_HPP_
