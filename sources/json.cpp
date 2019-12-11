// Copyright 2019 DM00n <teamvortex@yandex.ru>
#include "json.h"

bool Json::is_empty(char s) const {
    return (s == ' ') || (s == '\n') || (s == '\t');
}

bool Json::is_bool(std::string s, char c, int j) const {
    if (c == 'f') {
        char ex[6];
        for (int i = 0; i < 5; i++) {
            ex[i] = s[j + i];
        }
        ex[5] = '\0';
        return strcmp(ex, "false") == 0;
    }
    if (c == 't') {
        char ex[5];
        for (int i = 0; i < 4; ++i) {
            ex[i] = s[j + i];
        }
        ex[4] = '\0';
        return strcmp(ex, "true") == 0;
    }
    return false;
}

bool Json::is_letter(std::string s, unsigned j) const {
    return ((s[j] >= 'a') && (s[j] <= 'z')) ||
           ((s[j] >= 'A') && (s[j] <= 'Z'));
}

bool Json::is_number(std::string s, unsigned j) const {
    return (s[j] >= '0') && (s[j] <= '9');
}

int Json::detect_type(std::string s, unsigned j) const {
    char ch = s[j];
    if (ch == 'f') {
        if (is_bool(s, 'f', j)) return 0;
        return 5;
    }
    if (ch == 't') {
        if (is_bool(s, 't', j)) return 0;
        return 5;
    }
    if (ch == '\"') return 1;
    if (ch == '[') return 2;
    if (ch == '{') return 3;
    if (is_number(s, j)) return 4;
    return 5;
}

Json::Json(const std::string &s) { parse(s); }

bool Json::is_array() const {
    if (this->_json_vector.empty()) return false;
    return true;
}

bool Json::is_object() const {
    if (this->_json_map.empty()) return false;
    return true;
}

Json Json::parse(const std::string &s) {
    int figure_brace = 0,
            square_brace = 0,
            quotes = 0,
            skip_step = 0,
            type = -1;
    std::string key;
    std::any value;
    char symbol;
    for (unsigned i = 0; i < s.length(); ++i) {
        symbol = s[i];
        if (skip_step > 0) {
            skip_step--;
            continue;
        }
        if (!is_empty(symbol)) {
            if ((symbol == '{') && (quotes % 2 == 0)) figure_brace++;
            if ((symbol == '}') && (quotes % 2 == 0)) figure_brace--;
            if ((symbol == '[') && (quotes % 2 == 0)) square_brace++;
            if ((symbol == ']') && (quotes % 2 == 0)) square_brace--;
            if (symbol == '\"') quotes++;
            if ((figure_brace) || (symbol == '}')) {
                if (symbol == ':') {
                    unsigned j = i + 1;
                    while (j < s.length()) {
                        if (is_empty(s[j])) { j++; }
                        else
                            break;
                    }
                    type = detect_type(s, j);
                    if (type == 0) {
                        if (s[j] == 'f') {
                            value = false;
                            j += 4;
                            skip_step = j - i;
                        }
                        if (s[j] == 't') {
                            value = true;
                            j += 3;
                            skip_step = j - i;
                        }
                        continue;
                    }
                    if (type == 1) {
                        std::string value1;
                        int k = j + 1;
                        while (s[k] != '\"') {
                            value1 += s[k];
                            k++;
                        }
                        value = value1;
                        skip_step = k - i;
                        continue;
                    }
                    if (type == 2) {
                        std::string little_arr_string = "[";
                        int c = 1, k = j + 1;
                        while (c) {
                            if (s[k] == '[') c++;
                            if (s[k] == ']') {
                                c--;
                                little_arr_string += "]";
                                continue;
                            }
                            little_arr_string += s[k];
                            k++;
                        }
                        skip_step = k - i;
                        Json l_array(little_arr_string);
                        //l_array = l_array.parse(little_arr_string);
                        value = l_array;
                        continue;
                    }
                    if (type == 3) {
                        std::string little_obj_string = "{";
                        int c = 1, k = j + 1;
                        while (c) {
                            if (s[k] == '{') c++;
                            if (s[k] == '}') {
                                c--;
                                little_obj_string += "}";
                                continue;
                            }
                            little_obj_string += s[k];
                            k++;
                        }
                        skip_step = k - i + 1;
                        Json l_obj(little_obj_string);
                        //l_obj = l_obj.parse(little_obj_string);
                        value = l_obj;
                        continue;
                    }
                    if (type == 4) {
                        double value1 = static_cast<double>(s[j] - 48);
                        unsigned k = j + 1;
                        while (s[k] != ',') {
                            value1 = value1 * 10
                                     + static_cast<double>(s[k] - 48);
                            k++;
                        }
                        value = value1;
                        skip_step = k - i - 1;
                        continue;
                    }
                }
                if (quotes % 2 == 1) {
                    int j = i + 1;
                    while (s[j] != '\"') {
                        key += s[j];
                        j++;
                    }
                    skip_step = j - i - 1;
                    continue;
                }
                if ((quotes % 2 == 0)
                    && ((symbol == ',')
                        || (symbol == '}'))) {
                    _json_map.insert(std::pair<std::string,
                            std::any>(key, value));
                    value.reset();
                    key.clear();
                    continue;
                }
            }
            if ((square_brace) || (symbol == ']')) {
                unsigned j = i + 1;
                if ((quotes % 2 == 0)
                    && ((symbol == ',')
                        || (symbol == ']'))) {
                    _json_vector.push_back(value);
                    value.reset();
                    continue;
                }
                if (type == 0) {
                    if (s[j] == 'f') {
                        value = false;
                        j += 4;
                        skip_step = j - i;
                    }
                    if (s[j] == 't') {
                        value = false;
                        j += 3;
                        skip_step = j - i;
                    }
                }
                if (type == 1) {
                    if (quotes % 2 == 0) continue;
                    std::string value1;
                    while (s[j] != '\"') {
                        value1 += s[j];
                        j++;
                    }
                    value = value1;
                    skip_step = j - i - 1;
                    continue;
                }
                if (type == 2) {
                    std::string little_arr_string;
                    int c = 1, k = i;
                    while (c) {
                        if (s[k] == '[') c++;
                        if (s[k] == ']') {
                            c--;
                            little_arr_string += "]";
                            continue;
                        }
                        little_arr_string += s[k];
                        k++;
                    }
                    skip_step = k - i;
                    Json l_array(little_arr_string);
                    //l_array = l_array.parse(little_arr_string);
                    value = l_array;
                    continue;
                }
                if (type == 3) {
                    std::string little_arr_string = "{";
                    int c = 1, k = j + 1;
                    while (c) {
                        if (s[k] == '{') c++;
                        if (s[k] == '}') {
                            c--;
                            little_arr_string += "}";
                            continue;
                        }
                        little_arr_string += s[k];
                        k++;
                    }
                    skip_step = k - i;
                    Json l_arr(little_arr_string);
                    //l_arr = l_arr.parse(little_arr_string);
                    value = l_arr;
                    continue;
                }
                if (type == 4) {
                    int value1 = symbol - 48;
                    unsigned l = i + 1;
                    while ((s[l] != ',')
                           && (s[l] != ']')
                           && (!is_empty(s[l]))) {
                        value1 = value1 * 10 + (s[l] - 48);
                        l++;
                    }
                    skip_step = l - i - 1;
                    value = value1;
                    continue;
                }
                if (symbol == '[') {
                    while (j < s.length()) {
                        if (is_empty(s[j])) { j++; }
                        else
                            break;
                    }
                    type = detect_type(s, j);//0-bool, 1-str, 2-mas, 3-obj, 4-int
                    continue;
                }
            }
        }
    }
    return *this;
}

Json Json::parseFile(const std::string &path_to_file) {
    std::ifstream in;
    std::string line, str;
    in.open(path_to_file);
    if (!in) {
        std::cout << "error opening file";
        return *this;
    }
    while (getline(in, line)) {
        str += line + '\n';
    }
    in.close();
    parse(str);
    return *this;
}

void Json::print() {
    if (_json_vector.empty()) {
        for (auto it = _json_map.begin(); it != _json_map.end(); ++it) {
            std::cout << it->first << ": ";
            if ((it->second.type().name())[0] == 'b') {
                std::cout << std::any_cast<bool>(it->second) << '\n';
            } else if ((it->second.type().name())[0] == 'd') {
                std::cout << std::any_cast<double>(it->second) << '\n';
            } else if ((it->second.type().name())[0] == 'N') {
                std::cout << std::any_cast<std::string>(it->second) << '\n';
            } else if ((it->second.type().name())[0] == '4') {
                std::cout << std::endl << "{" << std::endl;
                (std::any_cast<Json>(it->second)).print();
                std::cout << "}" << std::endl;
            }
        }
    } else {
        for (unsigned i = 0; i < _json_vector.size(); ++i) {
            if ((_json_vector[i].type().name())[0] == 'b') {
                std::cout << std::any_cast<bool>(_json_vector[i])
                          << std::endl;
            } else if ((_json_vector[i].type().name())[0] == 'i') {
                std::cout << std::any_cast<int>(_json_vector[i])
                          << std::endl;
            } else if ((_json_vector[i].type().name())[0] == 'N') {
                std::cout << std::any_cast<std::string>(_json_vector[i])
                          << std::endl;
            } else if ((_json_vector[i].type().name())[0] == '4') {
                std::cout << std::endl << "{" << std::endl;
                (std::any_cast<Json>(_json_vector[i])).print();
                std::cout << "}" << std::endl;
            }
        }
    }
}

std::any &Json::operator[](const std::string &key) {
    if (_json_vector.empty())
        return _json_map.find(key)->second;
    else
    {
        throw "Error: exemplar is array";
    }
}

std::any &Json::operator[](int index) {
    if (_json_vector.empty())
    {
        throw "Error: exemplar is object";
    } else {
        return _json_vector[index];
    }
}

Json::Json() = default;
