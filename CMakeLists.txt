cmake_minimum_required(VERSION 3.12)
project(sjkxq_json VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 创建接口库目标（纯头文件库）
add_library(sjkxq_json INTERFACE)

# 添加头文件目录
target_include_directories(sjkxq_json
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 启用测试
option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

# 安装配置
install(
    DIRECTORY include/sjkxq_json
    DESTINATION include
)

install(
    TARGETS sjkxq_json
    EXPORT sjkxq_jsonTargets
    INCLUDES DESTINATION include
)

# 导出目标
install(
    EXPORT sjkxq_jsonTargets
    FILE sjkxq_jsonConfig.cmake
    NAMESPACE sjkxq_json::
    DESTINATION lib/cmake/sjkxq_json
)

# 生成包配置文件
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sjkxq_jsonConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sjkxq_jsonConfig.cmake
    INSTALL_DESTINATION lib/cmake/sjkxq_json
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sjkxq_jsonConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)