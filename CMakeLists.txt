cmake_minimum_required(VERSION 3.12)
project(sjkxq_json VERSION 1.0.0 LANGUAGES CXX)

# 添加cmake/modules目录到模块搜索路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# 设置默认构建类型
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # 基础编译选项
    add_compile_options(-Wall -Wextra -Wpedantic)
    
    # Debug特定选项
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0 -DDEBUG)
        # 启用地址消毒器
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
    
    # Release特定选项
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
        # 启用更积极的优化
        add_compile_options(-ffast-math -march=native)
    endif()
    
    # RelWithDebInfo特定选项
    if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(-O2 -g -DNDEBUG)
        # 保持帧指针以获得更好的调试信息
        add_compile_options(-fno-omit-frame-pointer)
    endif()
    
    # MinSizeRel特定选项
    if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        add_compile_options(-Os -DNDEBUG)
        # 启用链接时优化以进一步减小大小
        add_compile_options(-flto)
        add_link_options(-flto)
    endif()
elseif(MSVC)
    # MSVC基础编译选项
    add_compile_options(/W4)
    
    # Debug特定选项
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi /RTC1 /DDEBUG)
    endif()
    
    # Release特定选项
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /DNDEBUG)
        # 启用全程序优化
        add_compile_options(/GL)
        add_link_options(/LTCG)
    endif()
    
    # RelWithDebInfo特定选项
    if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(/O2 /Zi /DNDEBUG)
        # 生成完整的调试信息
        add_compile_options(/DEBUG:FULL)
    endif()
    
    # MinSizeRel特定选项
    if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        add_compile_options(/O1 /DNDEBUG)
    endif()
endif()

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
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(ENABLE_COMPLEXITY_CHECK "Enable code complexity checking with Lizard" ON)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

# 代码复杂度检查
if(ENABLE_COMPLEXITY_CHECK)
    find_package(Lizard)
    if(LIZARD_FOUND)
        file(GLOB_RECURSE SOURCES 
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        )
        lizard_add_target(complexity_check
            SOURCES ${SOURCES}
            OUTPUT_FILE "${CMAKE_BINARY_DIR}/complexity_report.txt"
        )
        message(STATUS "Code complexity check target 'complexity_check' is available")
    endif()
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
    FILE sjkxq_jsonTargets.cmake
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

# 安装生成的配置文件
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/sjkxq_jsonConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/sjkxq_jsonConfigVersion.cmake
    DESTINATION lib/cmake/sjkxq_json
)

# 生成并安装pkg-config文件
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sjkxq_json.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/sjkxq_json.pc
    @ONLY
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/sjkxq_json.pc
    DESTINATION lib/pkgconfig
)

# 安装选项
option(INSTALL_EXAMPLES "Install examples" OFF)
option(INSTALL_DOCS "Install documentation" OFF)

# 安装README和LICENSE文件
install(
    FILES README.md
    DESTINATION share/doc/sjkxq_json
)

# 如果存在LICENSE文件，也安装它
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    install(
        FILES LICENSE
        DESTINATION share/doc/sjkxq_json
    )
endif()