#ifndef SJKXQ_JSON_SERIALIZER_HPP
#define SJKXQ_JSON_SERIALIZER_HPP

#include "value.hpp"
#include <string>
#include <sstream>
#include <iomanip>

namespace sjkxq_json {

class Serializer {
public:
    // 序列化选项
    struct Options {
        bool pretty_print;  // 是否美化输出
        int indent_spaces;  // 缩进空格数
        
        Options() : pretty_print(false), indent_spaces(2) {}
        explicit Options(bool pretty) : pretty_print(pretty), indent_spaces(2) {}
        Options(bool pretty, int spaces) : pretty_print(pretty), indent_spaces(spaces) {}
    };

    // 将Value序列化为JSON字符串
    static std::string serialize(const Value& value, const Options& options = Options()) {
        Serializer serializer(options);
        return serializer.serialize_value(value);
    }

private:
    Serializer(const Options& options) : options_(options), indent_level_(0) {}

    // 添加缩进
    void append_indent(std::ostringstream& oss) const {
        if (options_.pretty_print) {
            oss << std::string(indent_level_ * options_.indent_spaces, ' ');
        }
    }

    // 添加换行
    void append_newline(std::ostringstream& oss) const {
        if (options_.pretty_print) {
            oss << '\n';
        }
    }

    // 序列化Value
    std::string serialize_value(const Value& value) {
        std::ostringstream oss;
        
        switch (value.type()) {
            case ValueType::Null:
                oss << "null";
                break;
            case ValueType::Boolean:
                oss << (value.as_boolean() ? "true" : "false");
                break;
            case ValueType::Number: {
                double num = value.as_number();
                // 检查是否为整数
                if (std::floor(num) == num && num <= static_cast<double>(INT_MAX) && num >= static_cast<double>(INT_MIN)) {
                    oss << static_cast<int>(num);
                } else {
                    // 使用固定表示法，避免科学计数法
                    oss << std::fixed << std::setprecision(10) << num;
                    // 移除尾随的0和小数点
                    std::string str = oss.str();
                    size_t decimal_pos = str.find('.');
                    if (decimal_pos != std::string::npos) {
                        size_t last_non_zero = str.find_last_not_of('0');
                        if (last_non_zero == decimal_pos) {
                            str.erase(decimal_pos);
                        } else {
                            str.erase(last_non_zero + 1);
                        }
                    }
                    return str;
                }
                break;
            }
            case ValueType::String:
                serialize_string(oss, value.as_string());
                break;
            case ValueType::Array:
                serialize_array(oss, value.as_array());
                break;
            case ValueType::Object:
                serialize_object(oss, value.as_object());
                break;
        }
        
        return oss.str();
    }

    // 序列化字符串
    void serialize_string(std::ostringstream& oss, const std::string& str) {
        oss << '"';
        for (char c : str) {
            switch (c) {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '/': oss << "\\/"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        // 控制字符使用\uXXXX格式
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') 
                            << static_cast<int>(static_cast<unsigned char>(c)) << std::dec;
                    } else {
                        oss << c;
                    }
                    break;
            }
        }
        oss << '"';
    }

    // 序列化数组
    void serialize_array(std::ostringstream& oss, const Value::Array& array) {
        oss << '[';
        
        if (array.empty()) {
            oss << ']';
            return;
        }
        
        if (options_.pretty_print) {
            indent_level_++;
            append_newline(oss);
        }
        
        for (size_t i = 0; i < array.size(); ++i) {
            if (options_.pretty_print) {
                append_indent(oss);
            }
            
            oss << serialize_value(array[i]);
            
            if (i < array.size() - 1) {
                oss << ',';
            }
            
            if (options_.pretty_print) {
                append_newline(oss);
            }
        }
        
        if (options_.pretty_print) {
            indent_level_--;
            append_indent(oss);
        }
        
        oss << ']';
    }

    // 序列化对象
    void serialize_object(std::ostringstream& oss, const Value::Object& object) {
        oss << '{';
        
        if (object.empty()) {
            oss << '}';
            return;
        }
        
        if (options_.pretty_print) {
            indent_level_++;
            append_newline(oss);
        }
        
        size_t i = 0;
        for (const auto& pair : object) {
            if (options_.pretty_print) {
                append_indent(oss);
            }
            
            serialize_string(oss, pair.first);
            oss << ':';
            
            if (options_.pretty_print) {
                oss << ' ';
            }
            
            oss << serialize_value(pair.second);
            
            if (i < object.size() - 1) {
                oss << ',';
            }
            
            if (options_.pretty_print) {
                append_newline(oss);
            }
            
            ++i;
        }
        
        if (options_.pretty_print) {
            indent_level_--;
            append_indent(oss);
        }
        
        oss << '}';
    }

    Options options_;
    int indent_level_;
};

} // namespace sjkxq_json

#endif // SJKXQ_JSON_SERIALIZER_HPP