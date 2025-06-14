#!/usr/bin/env zsh

# 参数解析
zparseopts -D -E \
  h=SHOW_HELP -help=SHOW_HELP \
  c=CLEAN -clean=CLEAN \
  d=DEBUG -debug=DEBUG \
  r=RELEASE -release=RELEASE \
  w=REL_WITH_DEB_INFO -relwithdebinfo=REL_WITH_DEB_INFO \
  m=MIN_SIZE_REL -minsizerel=MIN_SIZE_REL \
  j:=JOBS -jobs:=JOBS \
  t:=THRESHOLD -threshold:=THRESHOLD \
  l:=LENGTH -length:=LENGTH \
  a:=ARGS -arguments:=ARGS \
  o=OUTPUT_FILE -output-file:=OUTPUT_FILE

# 显示帮助信息
show_help() {
  echo "用法: $0 [选项]"
  echo "选项:"
  echo "  -h, --help              显示帮助信息"
  echo "  -c, --clean             清理构建目录"
  echo "构建类型选项:"
  echo "  -d, --debug             使用Debug构建(默认)"
  echo "  -r, --release           使用Release构建"
  echo "  -w, --relwithdebinfo    使用RelWithDebInfo构建"
  echo "  -m, --minsizerel        使用MinSizeRel构建"
  echo "复杂度检查选项:"
  echo "  -t, --threshold N       设置循环复杂度阈值(默认: 10)"
  echo "  -l, --length N          设置函数长度阈值(默认: 100)"
  echo "  -a, --arguments N       设置函数参数数量阈值(默认: 5)"
  echo "  -o, --output-file FILE  将结果输出到文件"
  echo "其他选项:"
  echo "  -j, --jobs N            设置并行构建任务数"
  echo ""
  echo "构建类型说明:"
  echo "  Debug           - 包含调试信息，无优化"
  echo "  Release         - 开启完全优化，无调试信息"
  echo "  RelWithDebInfo  - 开启优化但保留调试信息"
  echo "  MinSizeRel      - 优化以最小化代码体积"
  echo ""
  echo "示例:"
  echo "  $0                      使用默认设置运行代码复杂度检查"
  echo "  $0 -t 15                设置循环复杂度阈值为15"
  echo "  $0 -l 150 -a 7          设置函数长度阈值为150，参数数量阈值为7"
  echo "  $0 -o complexity.txt    将结果输出到complexity.txt文件"
  echo "  $0 -c                   清理构建目录"
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
CMAKE_OPTS=()
JOBS=()
CCN_THRESHOLD=10
FUNC_LENGTH=100
FUNC_ARGS=5
OUTPUT_FILE=""

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

# 启用代码复杂度检查
CMAKE_OPTS+=(-DENABLE_COMPLEXITY_CHECK=ON)

# 设置并行任务数
if [[ -n "$JOBS" ]]; then
  JOBS=("--parallel" "${JOBS[2]}")
fi

# 设置复杂度阈值
if [[ -n "$THRESHOLD" ]]; then
  CCN_THRESHOLD="${THRESHOLD[2]}"
fi

# 设置函数长度阈值
if [[ -n "$LENGTH" ]]; then
  FUNC_LENGTH="${LENGTH[2]}"
fi

# 设置函数参数数量阈值
if [[ -n "$ARGS" ]]; then
  FUNC_ARGS="${ARGS[2]}"
fi

# 设置输出文件
if [[ -n "$OUTPUT_FILE" ]]; then
  OUTPUT_FILE="${OUTPUT_FILE[2]}"
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

# 检查Lizard是否安装
check_lizard() {
  if ! command -v lizard &> /dev/null; then
    error "Lizard工具未安装，请选择以下方式之一安装：

    # 方式1: 使用系统包管理器（如果可用）
    sudo apt-get update && sudo apt-get install -y python3-lizard

    # 方式2: 使用Python虚拟环境（推荐）
    # 确保已安装python3-full和python3-venv
    sudo apt-get install -y python3-full python3-venv
    # 创建虚拟环境
    python3 -m venv ~/.venv/lizard
    # 激活虚拟环境并安装lizard
    source ~/.venv/lizard/bin/activate
    pip install lizard
    # 创建符号链接到系统路径（可选）
    sudo ln -s ~/.venv/lizard/bin/lizard /usr/local/bin/lizard

    # 方式3: 使用pipx（适用于应用程序）
    # 安装pipx
    sudo apt-get install -y pipx
    # 使用pipx安装lizard
    pipx install lizard

    安装完成后，请重新运行此脚本。"
  else
    info "Lizard工具已安装"
  fi
}

# 运行代码复杂度检查
run_complexity_check() {
  info "运行代码复杂度检查..."
  info "阈值设置: 循环复杂度=${CCN_THRESHOLD}, 函数长度=${FUNC_LENGTH}, 参数数量=${FUNC_ARGS}"
  
  # 检查Lizard是否安装
  check_lizard
  
  # 准备Lizard命令参数
  local lizard_cmd=(
    lizard
    "${0:a:h}/include"
    "${0:a:h}/test"
    --CCN "${CCN_THRESHOLD}"
    --length "${FUNC_LENGTH}"
    --arguments "${FUNC_ARGS}"
    --exclude "*_test.cpp"
    --exclude "build/*"
    -w
  )
  
  # 处理输出文件
  if [[ -n "${OUTPUT_FILE}" ]]; then
    info "将结果输出到文件: ${OUTPUT_FILE}"
    "${lizard_cmd[@]}" > "${OUTPUT_FILE}" || {
      warn "发现超过阈值的复杂代码 - 结果已保存到 ${OUTPUT_FILE}"
      return 0
    }
  else
    "${lizard_cmd[@]}" || {
      warn "发现超过阈值的复杂代码"
      return 0
    }
  fi
  
  success "未发现超过阈值的复杂代码"
  return 0
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

  # 运行代码复杂度检查
  run_complexity_check

  success "代码复杂度检查完成!"
}

# 执行主函数
main "$@"