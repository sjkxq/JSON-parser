#include <sjkxq_json/json.hpp>
#include <cstdint>
#include <cstdlib>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    try {
        // 将输入数据转换为字符串
        std::string input(reinterpret_cast<const char*>(data), size);
        
        // 尝试解析JSON
        sjkxq_json::Value value = sjkxq_json::Parser::parse(input);
        
        // 可选：将解析后的JSON重新序列化为字符串
        // std::string output = sjkxq_json::Serializer::serialize(value);
    } catch (...) {
        // 捕获所有异常，fuzzer会记录导致崩溃的输入
    }
    return 0;
}