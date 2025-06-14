#include <sjkxq_json/parser.hpp>
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using sjkxq_json::Parser;

// 测试超大JSON解析
TEST(BoundaryParserTest, HugeJson) {
    // 生成一个超大的JSON字符串 (约1MB)
    string huge_json = "{\"array\":[";
    for (int i = 0; i < 10000; ++i) {
        huge_json += (i > 0 ? "," : "") + to_string(i);
    }
    huge_json += "]}";

    EXPECT_NO_THROW(Parser::parse(huge_json));
}

// 测试特殊字符处理
TEST(BoundaryParserTest, SpecialCharacters) {
    vector<string> test_cases = {
        "{\"str\":\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"}",  // 转义字符
        "{\"unicode\":\"\u3042\u3044\u3046\"}",     // Unicode字符
        "{\"empty\":\"\"}",                         // 空字符串
        "{\"null\":null}",                          // null值
        "{\"bool\":true}",                          // 布尔值
        "{\"num\":1.234e-56}"                       // 科学计数法数字
    };

    for (const auto& test_case : test_cases) {
        EXPECT_NO_THROW(Parser::parse(test_case));
    }
}

// 测试深度嵌套JSON
TEST(BoundaryParserTest, DeeplyNested) {
    string nested_json;
    for (int i = 0; i < 100; ++i) {
        nested_json += "{\"level\":" + to_string(i) + ",";
    }
    nested_json += "\"value\":42";
    for (int i = 0; i < 100; ++i) {
        nested_json += "}";
    }

    EXPECT_NO_THROW(Parser::parse(nested_json));
}

// 测试边界数值
TEST(BoundaryParserTest, BoundaryNumbers) {
    vector<string> test_cases = {
        "{\"min\":-9223372036854775808}",  // 64位最小整数
        "{\"max\":9223372036854775807}",   // 64位最大整数
        "{\"float\":3.4028235e38}",        // 最大float值
        "{\"small\":1.1754944e-38}",       // 最小float值
        "{\"zero\":0}",
        "{\"tiny\":0.0000000000000001}"
    };

    for (const auto& test_case : test_cases) {
        EXPECT_NO_THROW(Parser::parse(test_case));
    }
}