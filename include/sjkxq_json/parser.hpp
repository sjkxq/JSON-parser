#ifndef SJKXQ_JSON_PARSER_HPP
#define SJKXQ_JSON_PARSER_HPP

#include "value.hpp"
#include <string>
#include <string_view>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include <cstdint>

namespace sjkxq_json {

class Parser {
public:
    // 从字符串解析JSON
    static Value parse(std::string_view json) {
        Parser parser(json);
        return parser.parse_value();
    }

private:
    Parser(std::string_view json) : json_(json), pos_(0) {}

    // 跳过空白字符
    void skip_whitespace() {
        while (pos_ < json_.size() && std::isspace(json_[pos_])) {
            ++pos_;
        }
    }

    // 检查当前字符并前进
    bool consume(char c) {
        skip_whitespace();
        if (pos_ < json_.size() && json_[pos_] == c) {
            ++pos_;
            return true;
        }
        return false;
    }

    // 解析JSON值
    Value parse_value() {
        skip_whitespace();
        if (pos_ >= json_.size()) {
            throw std::runtime_error("Unexpected end of input");
        }

        char c = json_[pos_];
        if (c == 'n') return parse_null();
        if (c == 't' || c == 'f') return parse_boolean();
        if (c == '"') return parse_string();
        if (c == '[') return parse_array();
        if (c == '{') return parse_object();
        if (c == '-' || std::isdigit(c)) return parse_number();

        throw std::runtime_error("Unexpected character: " + std::string(1, c));
    }

    // 解析null
    Value parse_null() {
        if (pos_ + 3 < json_.size() && 
            json_[pos_] == 'n' && 
            json_[pos_ + 1] == 'u' && 
            json_[pos_ + 2] == 'l' && 
            json_[pos_ + 3] == 'l') {
            pos_ += 4;
            return Value();
        }
        throw std::runtime_error("Invalid null value");
    }

    // 解析布尔值
    Value parse_boolean() {
        if (pos_ + 3 < json_.size() && 
            json_[pos_] == 't' && 
            json_[pos_ + 1] == 'r' && 
            json_[pos_ + 2] == 'u' && 
            json_[pos_ + 3] == 'e') {
            pos_ += 4;
            return Value(true);
        }
        if (pos_ + 4 < json_.size() && 
            json_[pos_] == 'f' && 
            json_[pos_ + 1] == 'a' && 
            json_[pos_ + 2] == 'l' && 
            json_[pos_ + 3] == 's' && 
            json_[pos_ + 4] == 'e') {
            pos_ += 5;
            return Value(false);
        }
        throw std::runtime_error("Invalid boolean value");
    }

    // 解析字符串
    Value parse_string() {
        if (!consume('"')) {
            throw std::runtime_error("Expected '\"'");
        }

        std::string result;
        while (pos_ < json_.size() && json_[pos_] != '"') {
            char c = json_[pos_++];
            if (c == '\\') {
                if (pos_ >= json_.size()) {
                    throw std::runtime_error("Unexpected end of input in string");
                }
                c = json_[pos_++];
                switch (c) {
                    case '"': result.push_back('"'); break;
                    case '\\': result.push_back('\\'); break;
                    case '/': result.push_back('/'); break;
                    case 'b': result.push_back('\b'); break;
                    case 'f': result.push_back('\f'); break;
                    case 'n': result.push_back('\n'); break;
                    case 'r': result.push_back('\r'); break;
                    case 't': result.push_back('\t'); break;
                    case 'u': {
                        // 解析4位十六进制Unicode
                        if (pos_ + 3 >= json_.size()) {
                            throw std::runtime_error("Unexpected end of input in Unicode escape");
                        }
                        std::string hex(json_.substr(pos_, 4));
                        pos_ += 4;
                        
                        // 简单处理：只支持基本多语言平面（BMP）
                        uint16_t code_point = static_cast<uint16_t>(std::strtol(hex.c_str(), nullptr, 16));
                        
                        // UTF-8编码
                        if (code_point < 0x80) {
                            // 1字节
                            result.push_back(static_cast<char>(code_point));
                        } else if (code_point < 0x800) {
                            // 2字节
                            result.push_back(static_cast<char>(0xC0 | (code_point >> 6)));
                            result.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
                        } else {
                            // 3字节
                            result.push_back(static_cast<char>(0xE0 | (code_point >> 12)));
                            result.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
                            result.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
                        }
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid escape sequence: \\" + std::string(1, c));
                }
            } else {
                result.push_back(c);
            }
        }

        if (!consume('"')) {
            throw std::runtime_error("Unterminated string");
        }

        return Value(result);
    }

    // 解析数字
    Value parse_number() {
        size_t start_pos = pos_;
        bool is_negative = consume('-');
        
        // 整数部分
        if (consume('0')) {
            // 以0开头的数字不能有多个数字
            if (pos_ < json_.size() && std::isdigit(json_[pos_])) {
                throw std::runtime_error("Leading zeros not allowed");
            }
        } else {
            // 至少需要一个数字
            if (pos_ >= json_.size() || !std::isdigit(json_[pos_])) {
                throw std::runtime_error("Invalid number");
            }
            while (pos_ < json_.size() && std::isdigit(json_[pos_])) {
                ++pos_;
            }
        }

        // 小数部分
        bool has_fraction = false;
        if (consume('.')) {
            has_fraction = true;
            // 小数点后至少需要一个数字
            if (pos_ >= json_.size() || !std::isdigit(json_[pos_])) {
                throw std::runtime_error("Invalid number");
            }
            while (pos_ < json_.size() && std::isdigit(json_[pos_])) {
                ++pos_;
            }
        }

        // 指数部分
        bool has_exponent = false;
        if (pos_ < json_.size() && (json_[pos_] == 'e' || json_[pos_] == 'E')) {
            has_exponent = true;
            ++pos_;
            if (pos_ < json_.size() && (json_[pos_] == '+' || json_[pos_] == '-')) {
                ++pos_;
            }
            // 指数部分至少需要一个数字
            if (pos_ >= json_.size() || !std::isdigit(json_[pos_])) {
                throw std::runtime_error("Invalid number");
            }
            while (pos_ < json_.size() && std::isdigit(json_[pos_])) {
                ++pos_;
            }
        }

        std::string number_str{json_.substr(start_pos, pos_ - start_pos)};
        if (has_fraction || has_exponent) {
            return Value(std::stod(number_str));
        } else {
            // 对于整数，我们尝试使用int，但如果超出范围，则使用double
            try {
                return Value(std::stoi(number_str));
            } catch (const std::out_of_range&) {
                return Value(std::stod(number_str));
            }
        }
    }

    // 解析数组
    Value parse_array() {
        if (!consume('[')) {
            throw std::runtime_error("Expected '['");
        }

        Value::Array array;
        skip_whitespace();
        
        if (consume(']')) {
            return Value(array); // 空数组
        }

        while (true) {
            array.push_back(parse_value());
            skip_whitespace();
            
            if (consume(']')) {
                break;
            }
            
            if (!consume(',')) {
                throw std::runtime_error("Expected ',' or ']'");
            }
        }

        return Value(array);
    }

    // 解析对象
    Value parse_object() {
        if (!consume('{')) {
            throw std::runtime_error("Expected '{'");
        }

        Value::Object object;
        skip_whitespace();
        
        if (consume('}')) {
            return Value(object); // 空对象
        }

        while (true) {
            skip_whitespace();
            if (json_[pos_] != '"') {
                throw std::runtime_error("Expected string key");
            }
            
            Value key = parse_string();
            skip_whitespace();
            
            if (!consume(':')) {
                throw std::runtime_error("Expected ':'");
            }
            
            object[key.as_string()] = parse_value();
            skip_whitespace();
            
            if (consume('}')) {
                break;
            }
            
            if (!consume(',')) {
                throw std::runtime_error("Expected ',' or '}'");
            }
        }

        return Value(object);
    }

    std::string_view json_;
    size_t pos_;
};

} // namespace sjkxq_json

#endif // SJKXQ_JSON_PARSER_HPP