# 尝试使用find_package查找GoogleTest
find_package(GTest QUIET)

if(NOT GTest_FOUND)
  # 如果未找到，使用FetchContent获取
  message(STATUS "GoogleTest not found, fetching from source...")
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
  )
  FetchContent_MakeAvailable(googletest)
  
  # 设置别名以匹配find_package的目标名称
  if(NOT TARGET GTest::GTest)
    add_library(GTest::GTest ALIAS gtest)
    add_library(GTest::Main ALIAS gtest_main)
  endif()
endif()

# 创建测试可执行文件
add_executable(sjkxq_json_tests
    test_main.cpp
    unit_tests/value_test.cpp
)

# 设置C++标准要求
target_compile_features(sjkxq_json_tests PRIVATE cxx_std_11)

# 添加包含目录
target_include_directories(sjkxq_json_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# 链接GoogleTest和主项目
target_link_libraries(sjkxq_json_tests
    PRIVATE
    GTest::GTest
    GTest::Main
    sjkxq_json
)

# 添加单元测试
add_test(NAME sjkxq_json_tests
    COMMAND sjkxq_json_tests
)

# 设置测试属性
set_tests_properties(sjkxq_json_tests PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)

# 集成测试
add_executable(integration_tests
    integration_tests/integration_test.cpp
)

target_compile_features(integration_tests PRIVATE cxx_std_11)

target_include_directories(integration_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(integration_tests
    PRIVATE
    GTest::GTest
    GTest::Main
    sjkxq_json
)

add_test(NAME integration_tests
    COMMAND integration_tests
)

set_tests_properties(integration_tests PROPERTIES
    TIMEOUT 60
    LABELS "integration"
)

# 基准测试
find_package(benchmark QUIET)

if(NOT benchmark_FOUND)
  message(STATUS "Google Benchmark not found, fetching from source...")
  include(FetchContent)
  FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
  )
  FetchContent_MakeAvailable(benchmark)
  
  # 设置别名以匹配find_package的目标名称
  if(NOT TARGET benchmark::benchmark)
    add_library(benchmark::benchmark ALIAS benchmark)
  endif()
endif()

add_executable(benchmark_tests
    benchmark/benchmark.cpp
)

target_compile_features(benchmark_tests PRIVATE cxx_std_11)

target_include_directories(benchmark_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(benchmark_tests
    PRIVATE
    sjkxq_json
    benchmark::benchmark
)

# 添加基准测试标签
add_test(NAME benchmark_tests
    COMMAND benchmark_tests
)

set_tests_properties(benchmark_tests PROPERTIES
    LABELS "benchmark"
)