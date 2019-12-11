// Copyright 2019 DM00n <teamvortex@yandex.ru>
#ifndef INCLUDE_JSON_HPP_
#define INCLUDE_JSON_HPP_

#include <fstream>
#include <any>
#include <map>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>

class Json {
public:
    std::map <std::string, std::any> _json_map;
    std::vector <std::any> _json_vector;

    void print();

    int detect_type(std::string s, unsigned j) const;

    bool is_letter(std::string s,unsigned j) const;

    bool is_number(std::string s, unsigned j) const;

    bool is_bool(std::string s, char c, int j) const;

    bool is_empty(char s) const;

    explicit Json(const std::string &s);

    Json();

    bool is_array() const;

    bool is_object() const;

    std::any &operator[](const std::string &key);

    std::any &operator[](int index);

    Json parse(const std::string &s);

    Json parseFile(const std::string &path_to_file);
};

#endif  // INCLUDE_JSON_HPP_
