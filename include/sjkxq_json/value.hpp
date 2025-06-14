#ifndef SJKXQ_JSON_VALUE_HPP
#define SJKXQ_JSON_VALUE_HPP

#include <climits>
#include <cmath>
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility> // 为std::exchange添加
#include <vector>

namespace sjkxq_json {

// 前向声明
class Value;

// JSON值类型
enum class ValueType { Null, Boolean, Number, String, Array, Object };

// JSON值类
class Value {
public:
    using Array = std::vector<Value>;
    using Object = std::map<std::string, Value>;

    // 构造函数
    Value() : type_(ValueType::Null) {}
    Value(std::nullptr_t) : type_(ValueType::Null) {}
    Value(bool value) : type_(ValueType::Boolean), boolean_(value) {}
    Value(int value) : type_(ValueType::Number), number_(static_cast<double>(value)) {}
    Value(double value) : type_(ValueType::Number), number_(value) {}
    Value(const std::string& value) : type_(ValueType::String), string_(new std::string(value)) {}
    Value(std::string&& value)
        : type_(ValueType::String), string_(new std::string(std::move(value))) {}
    Value(const char* value) : type_(ValueType::String), string_(new std::string(value)) {}
    Value(const Array& value) : type_(ValueType::Array), array_(new Array(value)) {}
    Value(Array&& value) : type_(ValueType::Array), array_(new Array(std::move(value))) {}
    Value(const Object& value) : type_(ValueType::Object), object_(new Object(value)) {}
    Value(Object&& value) : type_(ValueType::Object), object_(new Object(std::move(value))) {}

    // 复制构造函数
    Value(const Value& other) : type_(other.type_) {
        switch (type_) {
        case ValueType::Boolean:
            boolean_ = other.boolean_;
            break;
        case ValueType::Number:
            number_ = other.number_;
            break;
        case ValueType::String:
            string_ = other.string_ ? new std::string(*other.string_) : nullptr;
            break;
        case ValueType::Array:
            array_ = other.array_ ? new Array(*other.array_) : nullptr;
            break;
        case ValueType::Object:
            object_ = other.object_ ? new Object(*other.object_) : nullptr;
            break;
        default:
            break;
        }
    }

    // 移动构造函数
    Value(Value&& other) noexcept : type_(other.type_) {
        switch (type_) {
        case ValueType::Boolean:
            boolean_ = other.boolean_;
            break;
        case ValueType::Number:
            number_ = other.number_;
            break;
        case ValueType::String:
            string_ = std::exchange(other.string_, nullptr);
            break;
        case ValueType::Array:
            array_ = std::exchange(other.array_, nullptr);
            break;
        case ValueType::Object:
            object_ = std::exchange(other.object_, nullptr);
            break;
        default:
            break;
        }
        other.type_ = ValueType::Null;
    }

    // 析构函数
    ~Value() {
        clear();
    }

    // 复制赋值运算符
    Value& operator=(const Value& other) {
        if (this != &other) {
            clear();
            type_ = other.type_;
            switch (type_) {
            case ValueType::Boolean:
                boolean_ = other.boolean_;
                break;
            case ValueType::Number:
                number_ = other.number_;
                break;
            case ValueType::String:
                string_ = other.string_ ? new std::string(*other.string_) : nullptr;
                break;
            case ValueType::Array:
                array_ = other.array_ ? new Array(*other.array_) : nullptr;
                break;
            case ValueType::Object:
                object_ = other.object_ ? new Object(*other.object_) : nullptr;
                break;
            default:
                break;
            }
        }
        return *this;
    }

    // 移动赋值运算符
    Value& operator=(Value&& other) noexcept {
        if (this != &other) {
            clear();
            type_ = other.type_;
            switch (type_) {
            case ValueType::Boolean:
                boolean_ = other.boolean_;
                break;
            case ValueType::Number:
                number_ = other.number_;
                break;
            case ValueType::String:
                string_ = std::exchange(other.string_, nullptr);
                break;
            case ValueType::Array:
                array_ = std::exchange(other.array_, nullptr);
                break;
            case ValueType::Object:
                object_ = std::exchange(other.object_, nullptr);
                break;
            default:
                break;
            }
            other.type_ = ValueType::Null;
        }
        return *this;
    }

    // 类型检查
    ValueType type() const {
        return type_;
    }
    bool is_null() const {
        return type_ == ValueType::Null;
    }
    bool is_boolean() const {
        return type_ == ValueType::Boolean;
    }
    bool is_number() const {
        return type_ == ValueType::Number;
    }
    bool is_string() const {
        return type_ == ValueType::String;
    }
    bool is_array() const {
        return type_ == ValueType::Array;
    }
    bool is_object() const {
        return type_ == ValueType::Object;
    }

    // 获取值
    bool as_boolean() const {
        if (!is_boolean())
            throw std::runtime_error("Value is not a boolean");
        return boolean_;
    }

    double as_number() const {
        if (!is_number())
            throw std::runtime_error("Value is not a number");
        return number_;
    }

    int as_int() const {
        if (!is_number())
            throw std::runtime_error("Value is not a number");
        double intpart;
        if (std::modf(number_, &intpart) != 0.0 || number_ > static_cast<double>(INT_MAX) ||
            number_ < static_cast<double>(INT_MIN))
            throw std::runtime_error("Value is not an integer or out of range");
        return static_cast<int>(intpart);
    }

    const std::string& as_string() const {
        if (!is_string() || !string_)
            throw std::runtime_error("Value is not a string");
        return *string_;
    }

    const Array& as_array() const {
        if (!is_array() || !array_)
            throw std::runtime_error("Value is not an array");
        return *array_;
    }

    const Object& as_object() const {
        if (!is_object() || !object_)
            throw std::runtime_error("Value is not an object");
        return *object_;
    }

    // 数组和对象的非常量访问
    Array& as_array() {
        if (!is_array() || !array_)
            throw std::runtime_error("Value is not an array");
        return *array_;
    }

    Object& as_object() {
        if (!is_object() || !object_)
            throw std::runtime_error("Value is not an object");
        return *object_;
    }

    // 数组访问
    Value& operator[](size_t index) {
        if (!is_array()) {
            clear();
            type_ = ValueType::Array;
            array_ = new Array();
        }
        if (index >= array_->size()) {
            array_->resize(index + 1);
        }
        return (*array_)[index];
    }

    const Value& operator[](size_t index) const {
        if (!is_array() || !array_ || index >= array_->size()) {
            static Value null_value;
            return null_value;
        }
        return (*array_)[index];
    }

    // 对象访问
    Value& operator[](const std::string& key) {
        if (!is_object()) {
            clear();
            type_ = ValueType::Object;
            object_ = new Object();
        }
        return (*object_)[key];
    }

    const Value& operator[](const std::string& key) const {
        if (!is_object() || !object_) {
            static Value null_value;
            return null_value;
        }
        auto it = object_->find(key);
        if (it == object_->end()) {
            static Value null_value;
            return null_value;
        }
        return it->second;
    }

    // 清除当前值
    void clear() {
        switch (type_) {
        case ValueType::String:
            delete string_;
            string_ = nullptr;
            break;
        case ValueType::Array:
            delete array_;
            array_ = nullptr;
            break;
        case ValueType::Object:
            delete object_;
            object_ = nullptr;
            break;
        default:
            break;
        }
        type_ = ValueType::Null;
    }

private:
    ValueType type_;
    union {
        bool boolean_;
        double number_;
        std::string* string_;
        Array* array_;
        Object* object_;
    };
};

} // namespace sjkxq_json

#endif // SJKXQ_JSON_VALUE_HPP