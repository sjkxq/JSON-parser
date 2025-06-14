#ifndef SJKXQ_JSON_JSON_HPP
#define SJKXQ_JSON_JSON_HPP

#include "parser.hpp"
#include "serializer.hpp"
#include "value.hpp"
#include <string_view>

namespace sjkxq_json {

// 解析JSON字符串
inline Value parse(std::string_view json) {
    return Parser::parse(json);
}

// 将Value序列化为JSON字符串
inline std::string stringify(const Value& value, bool pretty = false) {
    return Serializer::serialize(value, Serializer::Options(pretty));
}

// 将Value序列化为JSON字符串，带格式化选项
inline std::string stringify(const Value& value, const Serializer::Options& options) {
    return Serializer::serialize(value, options);
}

} // namespace sjkxq_json

#endif // SJKXQ_JSON_JSON_HPP