#ifndef INC_3SEM_PARSERJSON_JSON_H
#define INC_3SEM_PARSERJSON_JSON_H

#include <fstream>
#include <any>
#include <map>
#include <iostream>
#include <cstring>
#include <vector>
class Json {
private:
    std::map <std::string,std::any> _json_map;
    std::vector <std::any> _json_vector;
    std::string _json_string;
public:
    void print();
    int detect_type(unsigned j) const;
    bool is_letter(unsigned j) const;
    bool is_number(unsigned j) const;
    bool is_bool(char s, int j) const;
    bool is_empty (char s) const;
    Json(const std::string&s);
    Json();
    bool is_array() const;
    bool is_object() const;

    // Метод возвращает значение по ключу key, если экземпляр является JSON-объектом.
    // Значение может иметь один из следующих типов: Json, std::string, double, bool или быть пустым.
    // Если экземпляр является JSON-массивом, генерируется исключение.
    std::any& operator[](const std::string& key);

    // Метод возвращает значение по индексу index, если экземпляр является JSON-массивом.
    // Значение может иметь один из следующих типов: Json, std::string, double, bool или быть пустым.
    // Если экземпляр является JSON-объектом, генерируется исключение.
    std::any& operator[](int index);

    // Метод возвращает объект класса Json из строки, содержащей Json-данные.
    Json parse(const std::string& s);

    // Метод возвращает объекта класса Json из файла, содержащего Json-данные в текстовом формате.
    Json parseFile(const std::string& path_to_file);

};
#endif
