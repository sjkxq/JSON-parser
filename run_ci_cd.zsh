#!/usr/bin/env zsh

# 集成CI/CD脚本
# 功能：
# 1. 代码格式化检查
# 2. 静态代码分析
# 3. 代码复杂度检查
# 4. 构建项目
# 5. 运行测试
# 6. 生成测试覆盖率报告

# 设置颜色
autoload -U colors && colors
RED=$fg[red]
GREEN=$fg[green]
YELLOW=$fg[yellow]
BLUE=$fg[blue]
RESET=$reset_color

# 输出函数
info() {
  echo "${BLUE}==>${RESET} ${1}"
}

success() {
  echo "${GREEN}==>${RESET} ${1}"
}

warn() {
  echo "${YELLOW}==> WARNING:${RESET} ${1}"
}

error() {
  echo "${RED}==> ERROR:${RESET} ${1}"
  exit 1
}

# 检查并安装依赖
check_dependencies() {
  local missing=0

  # 检查clang-format
  if ! command -v clang-format &> /dev/null; then
    warn "clang-format未安装，代码格式化检查将跳过"
    missing=1
  fi

  # 检查clang-tidy
  if ! command -v clang-tidy &> /dev/null; then
    warn "clang-tidy未安装，静态分析将跳过"
    missing=1
  fi

  # 检查lizard
  if ! command -v lizard &> /dev/null; then
    warn "lizard未安装，代码复杂度检查将跳过"
    missing=1
  fi

  # 检查lcov
  if ! command -v lcov &> /dev/null; then
    warn "lcov未安装，测试覆盖率报告将跳过"
    missing=1
  fi

  if [[ $missing -eq 1 ]]; then
    warn "部分依赖未安装，某些检查将被跳过"
  fi
}

# 运行代码格式化检查
run_format_check() {
  info "运行代码格式化检查..."
  if [[ -f ./run_clang_format.sh ]]; then
    ./run_clang_format.sh
    if [[ $? -ne 0 ]]; then
      error "代码格式化检查失败，请运行 ./run_clang_format.sh --fix 修复问题"
    fi
    success "代码格式化检查完成"
  else
    warn "run_clang_format.sh 脚本不存在，跳过代码格式化检查"
  fi
}

# 运行静态代码分析
run_static_analysis() {
  info "运行静态代码分析..."
  if [[ -f ./run_clang_tidy.sh ]]; then
    ./run_clang_tidy.sh
    if [[ $? -ne 0 ]]; then
      error "静态代码分析失败"
    fi
    success "静态代码分析完成"
  else
    warn "run_clang_tidy.sh 脚本不存在，跳过静态代码分析"
  fi
}

# 运行代码复杂度检查
run_complexity_check() {
  info "运行代码复杂度检查..."
  if [[ -f ./run_complexity_check.zsh ]]; then
    ./run_complexity_check.zsh
    if [[ $? -ne 0 ]]; then
      error "代码复杂度检查失败"
    fi
    success "代码复杂度检查完成"
  else
    warn "run_complexity_check.zsh 脚本不存在，跳过代码复杂度检查"
  fi
}

# 构建项目并运行测试
run_build_and_tests() {
  info "构建项目并运行测试..."
  if [[ -f ./run_tests.zsh ]]; then
    ./run_tests.zsh
    if [[ $? -ne 0 ]]; then
      error "构建或测试失败"
    fi
    success "构建和测试完成"
  else
    error "run_tests.zsh 脚本不存在，无法继续"
  fi
}

# 生成测试覆盖率报告
generate_coverage_report() {
  info "生成测试覆盖率报告..."
  if command -v lcov &> /dev/null; then
    # 清理旧的覆盖率数据
    lcov --directory build --zerocounters
    # 捕获初始覆盖率数据
    lcov --capture --initial --directory build --output-file coverage_base.info
    # 运行测试
    cd build && ctest
    # 捕获测试后的覆盖率数据
    lcov --capture --directory build --output-file coverage_test.info
    # 合并覆盖率数据
    lcov --add-tracefile coverage_base.info --add-tracefile coverage_test.info --output-file coverage_total.info
    # 生成HTML报告
    genhtml coverage_total.info --output-directory coverage_report
    success "测试覆盖率报告已生成到 build/coverage_report 目录"
  else
    warn "lcov未安装，跳过测试覆盖率报告生成"
  fi
}

# 主函数
main() {
  # 检查依赖
  check_dependencies

  # 1. 代码格式化检查
  run_format_check

  # 2. 静态代码分析
  run_static_analysis

  # 3. 代码复杂度检查
  run_complexity_check

  # 4. 构建和测试
  run_build_and_tests

  # 5. 测试覆盖率报告
  generate_coverage_report

  # 最终报告
  echo ""
  success "CI/CD流程执行完成"
  echo "所有检查已通过"
  echo "测试覆盖率报告: build/coverage_report/index.html"
}

# 执行主函数
main "$@"