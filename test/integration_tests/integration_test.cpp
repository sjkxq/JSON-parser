#include <gtest/gtest.h>
#include <sjkxq_json/json.hpp>
#include <fstream>

using namespace sjkxq_json;

TEST(IntegrationTest, ParseAndStringify) {
    // 测试完整的解析和序列化流程
    std::string jsonStr = R"({
        "name": "John Doe",
        "age": 30,
        "isStudent": false,
        "courses": ["Math", "Science"],
        "address": {
            "street": "123 Main St",
            "city": "Anytown"
        }
    })";

    Value v = parse(jsonStr);
    EXPECT_EQ(v["name"].as_string(), "John Doe");
    EXPECT_EQ(v["age"].as_number(), 30);
    EXPECT_FALSE(v["isStudent"].as_boolean());
    
    // 测试序列化
    std::string serialized = stringify(v);
    Value v2 = parse(serialized);
    EXPECT_EQ(v2["name"].as_string(), "John Doe");
}

TEST(IntegrationTest, FileOperations) {
    // 测试文件读写集成
    Value original;
    original["test"] = "file integration";
    original["value"] = 42;
    
    // 写入文件
    std::ofstream out("test.json");
    out << stringify(original);
    out.close();
    
    // 读取文件
    std::ifstream in("test.json");
    std::string content((std::istreambuf_iterator<char>(in)), 
                        std::istreambuf_iterator<char>());
    Value parsed = parse(content);
    
    EXPECT_EQ(parsed["test"].as_string(), "file integration");
    EXPECT_EQ(parsed["value"].as_number(), 42);
    
    // 清理
    std::remove("test.json");
}