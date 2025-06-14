#!/bin/bash

# 确保build目录存在
mkdir -p build

# 进入build目录
cd build

# 生成带编译数据库的CMake配置
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# 回到项目根目录
cd ..

# 创建符号链接到根目录（如果不存在）
if [ ! -f compile_commands.json ]; then
    ln -s build/compile_commands.json .
fi

# 查找所有C++源文件
find include src test -name '*.cpp' -o -name '*.hpp' -o -name '*.h' 2>/dev/null | while read -r file; do
    echo "Analyzing: $file"
    clang-tidy "$file"
done

echo "Static analysis complete!"