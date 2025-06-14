# sjkxq_json

一个现代化的C++ JSON解析库，提供简单易用的API来解析、操作和序列化JSON数据。

## 特性

- 轻量级：header-only库，无需额外依赖
- 易于使用：简洁的API设计
- 高性能：高效的解析和序列化
- 标准兼容：符合JSON标准
- 现代C++：使用C++11及以上特性

## 安装

### 使用CMake

```bash
mkdir build && cd build
cmake ..
sudo make install
```

### 配置选项

在CMake配置时，可以使用以下选项：

```bash
cmake .. -DINSTALL_EXAMPLES=ON -DINSTALL_DOCS=ON
```

## 使用

### 在CMake项目中使用

在你的CMakeLists.txt中添加：

```cmake
find_package(sjkxq_json REQUIRED)
target_link_libraries(your_target PRIVATE sjkxq_json::sjkxq_json)
```

### 在非CMake项目中使用

对于使用pkg-config的项目：

```bash
pkg-config --cflags sjkxq_json
```

### 代码示例

```cpp
#include <sjkxq_json/json.hpp>
#include <iostream>

int main() {
    // 解析JSON字符串
    std::string json_str = R"({"name":"John", "age":30, "city":"New York"})";
    auto json = sjkxq_json::parse(json_str);
    
    // 访问JSON值
    std::string name = json["name"].get<std::string>();
    int age = json["age"].get<int>();
    
    // 修改JSON值
    json["age"] = 31;
    
    // 序列化回JSON字符串
    std::string serialized = sjkxq_json::serialize(json);
    std::cout << serialized << std::endl;
    
    return 0;
}
```

## 开发

### 运行测试

```bash
chmod +x run_tests.zsh
./run_tests.zsh -a
```

### 代码格式化

```bash
chmod +x run_clang_format.sh
./run_clang_format.sh
```

### 静态分析

```bash
chmod +x run_clang_tidy.sh
./run_clang_tidy.sh
```

### 代码复杂度检查

使用Lizard工具进行代码复杂度检查，可以检测：
- 循环复杂度（CCN）
- 函数长度
- 函数参数数量

运行复杂度检查：
```bash
chmod +x run_complexity_check.zsh
./run_complexity_check.zsh
```

支持的选项：
```bash
# 自定义复杂度阈值（CCN=15，函数长度=150行，参数数量=7）
./run_complexity_check.zsh -t 15 -l 150 -a 7

# 将结果输出到文件
./run_complexity_check.zsh -o complexity.txt

# 查看所有选项
./run_complexity_check.zsh --help
```

默认阈值：
- 循环复杂度（CCN）：10
- 函数长度：100行
- 函数参数数量：5

## 开发

### 代码复杂度检查

使用Lizard工具进行代码复杂度分析，可以检测：
- 循环复杂度（CCN）：衡量函数控制流的复杂度
- 函数长度：函数的总行数
- 参数数量：函数的参数个数

#### CI/CD自动化脚本

`run_ci_cd.zsh`脚本用于自动化执行完整的CI/CD流程，包括：
- 代码格式化检查
- 静态代码分析
- 代码复杂度检查
- 单元测试执行

##### 运行完整CI/CD流程
```bash
./run_ci_cd.zsh
```

##### 选择性执行
```bash
# 仅运行代码格式化检查
./run_ci_cd.zsh --format-only

# 跳过测试执行
./run_ci_cd.zsh --no-tests

# 详细模式(显示完整输出)
./run_ci_cd.zsh -v
```

#### 运行复杂度检查
```bash
./run_complexity_check.zsh
```

#### 自定义检查参数
```bash
# 设置循环复杂度阈值为15，函数长度阈值为150，参数数量阈值为7
./run_complexity_check.zsh -t 15 -l 150 -a 7

# 将结果输出到文件
./run_complexity_check.zsh -o complexity.txt
```

#### 阈值建议
- 循环复杂度：建议保持≤10
- 函数长度：建议保持≤100行
- 参数数量：建议保持≤5个