#!/bin/bash

# 脚本用于运行clang-format检查所有源文件并确保它们符合规范
# 使用方法：
#   ./run_clang_format.sh          # 检查模式，只报告问题但不修改文件
#   ./run_clang_format.sh --fix    # 修复模式，自动修复格式问题

# 设置颜色
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# 检查clang-format是否安装
if ! command -v clang-format &> /dev/null; then
    echo -e "${RED}错误: clang-format 未安装。请先安装clang-format。${NC}"
    exit 1
fi

# 检查.clang-format文件是否存在
if [ ! -f .clang-format ]; then
    echo -e "${RED}错误: .clang-format 文件不存在。${NC}"
    exit 1
fi

# 确定是检查模式还是修复模式
FIX_MODE=0
if [ "$1" == "--fix" ]; then
    FIX_MODE=1
    echo -e "${YELLOW}运行在修复模式 - 将自动修复格式问题${NC}"
else
    echo -e "${YELLOW}运行在检查模式 - 只报告问题但不修改文件${NC}"
    echo -e "使用 ${GREEN}./run_clang_format.sh --fix${NC} 来自动修复问题"
fi

# 查找所有C++源文件
SOURCE_FILES=$(find include test -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \) 2>/dev/null)

if [ -z "$SOURCE_FILES" ]; then
    echo -e "${RED}未找到任何源文件${NC}"
    exit 1
fi

# 统计信息
TOTAL_FILES=0
FORMATTED_FILES=0
PROBLEM_FILES=0

# 处理每个文件
for file in $SOURCE_FILES; do
    ((TOTAL_FILES++))
    
    if [ $FIX_MODE -eq 1 ]; then
        # 修复模式：直接修改文件
        clang-format -i "$file"
        echo -e "${GREEN}已格式化: ${NC}$file"
        ((FORMATTED_FILES++))
    else
        # 检查模式：比较格式化前后的差异
        DIFF=$(clang-format "$file" | diff -u "$file" -)
        if [ -n "$DIFF" ]; then
            echo -e "${RED}格式问题: ${NC}$file"
            ((PROBLEM_FILES++))
        else
            echo -e "${GREEN}格式正确: ${NC}$file"
        fi
    fi
done

# 输出统计信息
echo ""
echo -e "${YELLOW}统计信息:${NC}"
echo -e "检查的文件总数: $TOTAL_FILES"

if [ $FIX_MODE -eq 1 ]; then
    echo -e "已格式化的文件数: $FORMATTED_FILES"
    echo -e "${GREEN}所有文件现在符合格式规范${NC}"
else
    echo -e "有问题的文件数: $PROBLEM_FILES"
    if [ $PROBLEM_FILES -eq 0 ]; then
        echo -e "${GREEN}所有文件都符合格式规范${NC}"
    else
        echo -e "${RED}有 $PROBLEM_FILES 个文件不符合格式规范${NC}"
        echo -e "运行 ${GREEN}./run_clang_format.sh --fix${NC} 来自动修复问题"
    fi
fi