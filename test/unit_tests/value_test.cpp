#include <gtest/gtest.h>

#include "sjkxq_json/value.hpp"

TEST(ValueTest, DefaultConstructor)
{
    sjkxq_json::Value value;
    EXPECT_TRUE(value.is_null());
}

TEST(ValueTest, IntConstructor)
{
    sjkxq_json::Value value(42);
    EXPECT_TRUE(value.is_number());
    EXPECT_EQ(value.as_int(), 42);
}

TEST(ValueTest, StringConstructor)
{
    sjkxq_json::Value value("test");
    EXPECT_TRUE(value.is_string());
    EXPECT_EQ(value.as_string(), "test");
}

// 添加更多测试用例...