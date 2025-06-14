#!/usr/bin/env zsh

# 参数解析
zparseopts -D -E \
  h=SHOW_HELP -help=SHOW_HELP \
  a=RUN_ALL -all=RUN_ALL \
  u=RUN_UNIT -unit=RUN_UNIT \
  i=RUN_INTEGRATION -integration=RUN_INTEGRATION \
  b=RUN_BENCHMARK -benchmark=RUN_BENCHMARK \
  f=RUN_FUZZ -fuzz=RUN_FUZZ \
  c=CLEAN -clean=CLEAN \
  d=DEBUG -debug=DEBUG \
  r=RELEASE -release=RELEASE \
  w=REL_WITH_DEB_INFO -relwithdebinfo=REL_WITH_DEB_INFO \
  m=MIN_SIZE_REL -minsizerel=MIN_SIZE_REL \
  j:=JOBS -jobs:=JOBS

# 显示帮助信息
show_help() {
  echo "用法: $0 [选项]"
  echo "选项:"
  echo "  -h, --help              显示帮助信息"
  echo "  -a, --all              构建项目并运行所有测试(默认)"
  echo "  -u, --unit             只运行单元测试"
  echo "  -i, --integration      只运行集成测试"
  echo "  -b, --benchmark        只运行基准测试"
  echo "  -f, --fuzz            只运行模糊测试"
  echo "  -c, --clean           清理构建目录"
  echo "构建类型选项:"
  echo "  -d, --debug           使用Debug构建(默认)"
  echo "  -r, --release         使用Release构建"
  echo "  -w, --relwithdebinfo  使用RelWithDebInfo构建"
  echo "  -m, --minsizerel      使用MinSizeRel构建"
  echo "其他选项:"
  echo "  -j, --jobs N          设置并行构建任务数"
  echo ""
  echo "构建类型说明:"
  echo "  Debug           - 包含调试信息，无优化"
  echo "  Release         - 开启完全优化，无调试信息"
  echo "  RelWithDebInfo - 开启优化但保留调试信息"
  echo "  MinSizeRel     - 优化以最小化代码体积"
  echo ""
  echo "示例:"
  echo "  $0 -u                只运行单元测试"
  echo "  $0 -i -j4            运行集成测试，使用4个并行任务"
  echo "  $0 -w                使用RelWithDebInfo构建并运行所有测试"
  echo "  $0 -m -b             使用MinSizeRel构建并运行基准测试"
  echo "  $0 -c                清理构建目录"
}

# 彩色输出定义
autoload -U colors && colors
RED=$fg[red]
GREEN=$fg[green]
YELLOW=$fg[yellow]
BLUE=$fg[blue]
RESET=$reset_color

# 配置参数
BUILD_TYPE="Debug"
CTEST_ARGS=()
CMAKE_OPTS=()
JOBS=()

# 根据构建类型设置构建目录
if [[ "$BUILD_TYPE" == "Debug" ]]; then
  BUILD_DIR="${0:a:h}/build"
elif [[ "$BUILD_TYPE" == "Release" ]]; then
  BUILD_DIR="${0:a:h}/build_release"
elif [[ "$BUILD_TYPE" == "RelWithDebInfo" ]]; then
  BUILD_DIR="${0:a:h}/build_relwithdebinfo"
elif [[ "$BUILD_TYPE" == "MinSizeRel" ]]; then
  BUILD_DIR="${0:a:h}/build_minsizerel"
else
  BUILD_DIR="${0:a:h}/build"
fi

# 根据参数设置构建类型
if [[ -n "$RELEASE" ]]; then
  BUILD_TYPE="Release"
fi
if [[ -n "$REL_WITH_DEB_INFO" ]]; then
  BUILD_TYPE="RelWithDebInfo"
fi
if [[ -n "$MIN_SIZE_REL" ]]; then
  BUILD_TYPE="MinSizeRel"
fi
CMAKE_OPTS+=(-DCMAKE_BUILD_TYPE=${BUILD_TYPE})

# 设置并行任务数
if [[ -n "$JOBS" ]]; then
  JOBS=("--parallel" "${JOBS[2]}")
fi

# 打印带颜色的信息
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

# 清理构建目录
clean_build() {
  info "清理构建目录: ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}" || warn "清理构建目录失败"
}

# 构建项目
build_project() {
  # 1. 准备构建目录
  info "准备构建目录: ${BUILD_DIR}"
  mkdir -p "${BUILD_DIR}" || error "无法创建构建目录"

  # 2. 运行CMake
  info "运行CMake配置 (${BUILD_TYPE})"
  cd "${BUILD_DIR}" && cmake "${CMAKE_OPTS[@]}" .. || error "CMake配置失败"

  # 3. 编译项目
  info "编译项目"
  cmake --build . "${JOBS[@]}" || error "编译失败"
}

# 运行单元测试
run_unit_tests() {
  info "运行单元测试..."
  ctest --output-on-failure -L unit || error "单元测试失败"
}

# 运行集成测试
run_integration_tests() {
  info "运行集成测试..."
  ctest --output-on-failure -L integration || error "集成测试失败"
}

# 运行基准测试
run_benchmark_tests() {
  info "运行基准测试..."
  ctest --output-on-failure -L benchmark || error "基准测试失败"
}

# 运行模糊测试
run_fuzz_tests() {
  info "运行模糊测试..."
  ctest --output-on-failure -L fuzz || error "模糊测试失败"
}

# 运行所有测试
run_all_tests() {
  info "运行所有测试..."
  ctest --output-on-failure || error "测试失败"
}

# 主函数
main() {
  # 显示帮助信息
  if [[ -n "$SHOW_HELP" ]]; then
    show_help
    return 0
  fi

  # 清理构建目录
  if [[ -n "$CLEAN" ]]; then
    clean_build
    return 0
  fi

  # 构建项目
  build_project

  # 运行指定测试
  if [[ -n "$RUN_UNIT" ]]; then
    run_unit_tests
  elif [[ -n "$RUN_INTEGRATION" ]]; then
    run_integration_tests
  elif [[ -n "$RUN_BENCHMARK" ]]; then
    run_benchmark_tests
  elif [[ -n "$RUN_FUZZ" ]]; then
    run_fuzz_tests
  else
    # 默认行为: 运行所有测试
    run_all_tests
  fi

  success "操作完成!"
}

# 执行主函数
main "$@"