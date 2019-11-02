// Copyright 2019 DM00n <teamvortex@yandex.ru>
#include "json.hpp"

bool Json::is_empty(char s) const {
    return (s == ' ') || (s == '\n') || (s == '\t');
}

bool Json::is_bool(char s, int j) const {

    if (s == 'f') {
        char ex[6];
        for (int i = 0; i < 5; i++) {
            ex[i] = _json_string[j + i];
        }
        ex[5] = '\0';
        return strcmp(ex, "false") == 0;
    }
    if (s == 't') {
        char ex[5];
        for (int i = 0; i < 4; ++i) {
            ex[i] = _json_string[j + i];
        }
        ex[4] = '\0';
        return strcmp(ex, "true") == 0;
    }
    return false;
}

bool Json::is_letter(unsigned j) const {
    return ((_json_string[j] >= 'a') && (_json_string[j] <= 'z')) ||
           ((_json_string[j] >= 'A') && (_json_string[j] <= 'Z'));
}

bool Json::is_number(unsigned j) const {
    return (_json_string[j] >= '0') && (_json_string[j] <= '9');
}

int Json::detect_type(unsigned j) const {
    char ch = _json_string[j];
    if (ch == 'f') {
        if (is_bool('f', j)) return 0;
        return 5;
    }
    if (ch == 't') {
        if (is_bool('t', j)) return 0;
        return 5;
    }
    if (ch == '\"') return 1;
    if (ch == '[') return 2;
    if (ch == '{') return 3;
    if (is_number(j)) return 4;
    return 5;
}

Json::Json(const std::string &s) { _json_string = s; }

bool Json::is_array() const {
    char symbol;
    int l_square_brace = 0,
            r_square_brace = 0,
            l_figure_brace = 0,
            r_figure_brace = 0,
            mas_type = -1,
            quotes = 0,
            skip_step = 0;
    for (unsigned i = 0; i < _json_string.length(); ++i) {
        symbol = _json_string[i];
        if (skip_step != 0) {
            skip_step--;
            continue;
        }
        if (!is_empty(symbol)) {
            if ((symbol == '{') && (quotes % 2 == 0)) l_figure_brace++;
            if ((symbol == '}') && (quotes % 2 == 0)) r_figure_brace++;
            if ((symbol == '[') && (quotes % 2 == 0)) l_square_brace++;
            if ((symbol == ']') && (quotes % 2 == 0)) {
                r_square_brace++;
                if (l_square_brace - r_square_brace == 0) mas_type = -1;
            }
            if (symbol == '\"') quotes++;
            if (((l_figure_brace == 0)
                 && (symbol != '{'))
                && ((l_square_brace == 0)
                    && (symbol != '[')))
                return false;
            if ((symbol == '[') && (l_square_brace == 1) &&
                (mas_type = -1)) {
                unsigned j = i + 1;
                while ((j <= _json_string.length())
                       && (is_empty(_json_string[j]))) { j++; }
                mas_type = detect_type(j);
                continue;
            }
            if (symbol == ']') {
                unsigned j = i + 1;
                while ((j <= _json_string.length()) && (is_empty(_json_string[j]))) { j++; }
                if (_json_string[j] == '[') return false;
            }
            if (symbol != ',') {
                switch (mas_type) {
                    case 0 : {
                        if ((symbol != 'f') && (symbol != 't')) return false;
                        if (symbol == 'f') {
                            if (is_bool('f', i) == 0) return false;
                            unsigned j = i + 5;
                            while ((j < _json_string.length())
                                   && (is_empty(_json_string[j]))) { j++; }
                            if ((_json_string[j] != ',')
                                && (_json_string[j] != ']'))
                                return false;
                            skip_step = 4;
                        }
                        if (symbol == 't') {
                            if (is_bool('t', i) == 0) return false;
                            unsigned j = i + 4;
                            while ((j < _json_string.length())
                                   && (is_empty(_json_string[j]))) { j++; }
                            if ((_json_string[j] != ',')
                                && (_json_string[j] != ']'))
                                return false;
                            skip_step = 3;
                        }

                        break;
                    }
                    case 1 : {
                        if ((quotes % 2 == 0)
                            && (!((symbol == ',')
                                  || (symbol == ']')
                                  || (symbol == '\"'))))
                            return false;
                        if (quotes % 2 == 0) {
                            unsigned j = i + 1;
                            while ((j < _json_string.length())
                                   && (is_empty(_json_string[j]))) { j++; }
                            if ((_json_string[j] != ',')
                                && (_json_string[j] != ']'))
                                return false;
                        }
                        break;
                    }
                    case 2 : {
                        if ((symbol != '[') && (symbol != ']')) return false;
                        if ((symbol == '[') && (quotes % 2 == 0)) {
                            std::string little_arr = "[";
                            int c = 1, j = i + 1;
                            while (c) {
                                if (_json_string[j] == '[') c++;
                                if (_json_string[j] == ']') {
                                    c--;
                                    little_arr += "]";
                                    continue;
                                }
                                little_arr += _json_string[j];
                                j++;
                                skip_step++;
                            }
                            Json l_array(little_arr);
                            if (!l_array.is_array())return false;
                            continue;
                        }
                        break;
                    }
                    case 3 : {
                        if ((symbol != '{') && (symbol != '}')) return false;
                        if (symbol == '{') {
                            std::string little_obj = "{";
                            int c = 1, j = i + 1;
                            while (c) {
                                if (_json_string[j] == '{') c++;
                                if (_json_string[j] == '}') {
                                    c--;
                                    little_obj += "}";
                                    continue;
                                }
                                little_obj += _json_string[j];
                                j++;
                                skip_step++;
                            }
                            Json l_object(little_obj);
                            if (!l_object.is_object())return false;
                        }
                        if (symbol == '}') {
                            unsigned j = i + 1;
                            while ((j < _json_string.length())
                                   && (is_empty(_json_string[j]))) { j++; }
                            if ((_json_string[j] != ',')
                                && (_json_string[j] != ']'))
                                return false;
                        }
                        break;
                    }
                    case 4 : {
                        if (!is_number(i)) return false;
                        unsigned j = i + 1;
                        if (is_number(j)) break;
                        if (is_empty(_json_string[j])) {
                            while ((j <= _json_string.length())
                                   && (is_empty(_json_string[j]))) { j++; }
                            if ((_json_string[j] != ',')
                                && (_json_string[j] != ']'))
                                return false;
                        }
                        break;
                    }
                    case 5 : {
                        return false;
                    }
                }
            } else {
                unsigned j = i + 1;
                while ((j < _json_string.length())
                       && (is_empty(_json_string[j]))) { j++; }
                if (_json_string[j] == ']') return false;
            }
            continue;
        }
    }
    return !((l_square_brace + r_square_brace == 0)
             || (quotes % 2 == 1)
             || (l_square_brace > r_square_brace)
             || (l_figure_brace > r_figure_brace));
}

bool Json::is_object() const {
    int l_figure_brace = 0,
            r_figure_brace = 0,
            l_square_brace = 0,
            r_square_brace = 0,
            quotes = 0,
            skip_step = 0;
    char symbol;
    for (unsigned i = 0; i < _json_string.length(); ++i) {
        symbol = _json_string[i];
        if (skip_step != 0) {
            skip_step--;
            continue;
        }
        if (!is_empty(symbol)) {
            if ((symbol == '{') && (quotes % 2 == 0)) l_figure_brace++;
            if ((symbol == '}') && (quotes % 2 == 0)) r_figure_brace++;
            if ((symbol == '[') && (quotes % 2 == 0)) l_square_brace++;
            if ((symbol == ']') && (quotes % 2 == 0)) r_square_brace++;
            if (symbol == '\"') quotes++;
            if (((l_figure_brace == 0)
                 && (symbol != '{'))
                && ((l_square_brace == 0)
                    && (symbol != '[')))
                return false;
            if (symbol == '{') {
                unsigned j = i + 1;
                while (j < _json_string.length()) {
                    if (is_empty(_json_string[j])) { j++; }
                    else
                        break;
                }
                if (_json_string[j] == '{') return false;
            }
            if ((symbol == '{')
                && (quotes % 2 == 0)
                && (l_figure_brace != 1)) {
                std::string little_obj = "{";
                int c = 1, j = i + 1;
                while (c) {
                    if (_json_string[j] == '{') c++;
                    if (_json_string[j] == '}') {
                        c--;
                        little_obj += "}";
                        continue;
                    }
                    little_obj += _json_string[j];
                    j++;
                    skip_step++;
                }
                Json l_object(little_obj);
                if (!l_object.is_object())return false;
                continue;
            }
            if ((symbol == '[') && (quotes % 2 == 0)) {
                std::string little_arr = "[";
                int c = 1, j = i + 1;
                while (c) {
                    if (_json_string[j] == '[') c++;
                    if (_json_string[j] == ']') {
                        c--;
                        little_arr += "]";
                        continue;
                    }
                    little_arr += _json_string[j];
                    j++;
                    skip_step++;
                }
                Json l_array(little_arr);
                if (!l_array.is_array())return false;
                continue;
            }
            if ((quotes % 2 == 0) && (symbol == ':')) {
                unsigned j = i + 1;
                while (j < _json_string.length()) {
                    if (is_empty(_json_string[j])) {
                        j++;
                    } else
                        break;
                }
                char ch = _json_string[j];
                unsigned k = j + 1;
                if (ch == '\"') {
                    while ((k + 1 < _json_string.length())
                           && (_json_string[k] != '\"')) {
                        k++;
                    }
                    while (k + 1 < _json_string.length()) {
                        if (is_empty(_json_string[k + 1])) { k++; }
                        else
                            break;
                    }
                    if ((_json_string[k + 1] != ',')
                        && (_json_string[k + 1] != ']')
                        && (_json_string[k + 1] != '}'))
                        return false;
                }
                if ((ch != '\"') && (ch != '{') && (ch != '[')) {
                    if ((is_number(j))
                        && !((is_number(j + 1))
                             || (ch == ',')))
                        return false;
                }
                continue;
            }
            if ((quotes % 2 == 0) && (symbol == ',')) {
                unsigned j = i + 1;
                while ((j < _json_string.length())
                       && (is_empty(_json_string[j]))) { j++; }
                if (_json_string[j] != '\"') return false;
                continue;
            }
            if ((l_figure_brace > r_figure_brace)
                && (quotes % 2 == 0)
                && ((symbol == '}')
                    || (symbol == ']'))) {
                unsigned j = i + 1;
                while ((j < _json_string.length())
                       && (is_empty(_json_string[j]))) { j++; }
                if ((_json_string[j] != '}')
                    && (_json_string[j] != ']')
                    && (_json_string[j] != ','))
                    return false;
            }
            if ((quotes % 2 == 0) && (is_letter(i))) {
                if (symbol == 'f') {
                    if (is_bool('f', i) == 0) return false;
                    skip_step = 4;
                }
                if (symbol == 't') {
                    if (is_bool('t', i) == 0) return false;
                    skip_step = 3;
                }
                continue;
            }
            if ((quotes % 2 == 0) && ((symbol != '}')
                                      && (symbol != '{')
                                      && (symbol != '"')
                                      && (!is_number(i))
                                      && (symbol != '[')
                                      && (symbol != ']')))
                return false;
        }
    }
    return !((l_figure_brace - r_figure_brace != 0)
             || (l_figure_brace + r_figure_brace == 0)
             || (quotes % 2 == 1)
             || (l_square_brace > r_square_brace)
             || (l_figure_brace > r_figure_brace));
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
                    type = detect_type(j);
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
                        l_array = l_array.parse(_json_string);
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
                        l_obj = l_obj.parse(_json_string);
                        value = l_obj;
                        continue;
                    }
                    if (type == 4) {
                        double value1 = static_cast<double>(s[j] - 48);
                        unsigned k = j + 1;
                        while (s[k] != ',') {
                            value1 = value1 * 10 + static_cast<double>(s[k] - 48);
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
                    l_array = l_array.parse(_json_string);
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
                    l_arr = l_arr.parse(_json_string);
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
                    type = detect_type(j);//0-bool, 1-str, 2-mas, 3-obj, 4-int
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
    std::map<std::string, std::any>::iterator it = _json_map.find(key);
    if (_json_vector.empty()) throw;
    return it->second;
}

std::any &Json::operator[](int index) {
    if (!_json_map.empty()) throw;
    return _json_vector[index];
}

Json::Json() = default;
