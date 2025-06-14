# 添加构建选项
option(BUILD_TESTING "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_FUZZ_TESTS "Build fuzz tests" OFF)

if(NOT BUILD_TESTING AND NOT BUILD_BENCHMARKS AND NOT BUILD_FUZZ_TESTS)
    return()
endif()

# 尝试使用find_package查找GoogleTest
if(BUILD_TESTING)
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
        boundary_tests/boundary_parser_test.cpp
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

    # 添加边界测试
    add_test(NAME boundary_tests
        COMMAND sjkxq_json_tests --gtest_filter=BoundaryParserTest*
    )

    # 设置测试属性
    set_tests_properties(boundary_tests PROPERTIES
        TIMEOUT 60
        LABELS "boundary"
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
    
    # 复制测试数据文件
        file(GLOB_RECURSE TEST_DATA_FILES 
            "${CMAKE_CURRENT_SOURCE_DIR}/test_data/*"
            "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/test_data/*"
        )
    
        foreach(TEST_DATA_FILE ${TEST_DATA_FILES})
            file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${TEST_DATA_FILE})
            configure_file(${TEST_DATA_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${REL_PATH} COPYONLY)
        endforeach()
    endif()

    # 模糊测试
    if(BUILD_FUZZ_TESTS)
        # 检查编译器是否支持模糊测试
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # 添加模糊测试可执行文件
            add_executable(fuzz_parser
                fuzz_tests/fuzz_parser.cpp
            )

            # 设置C++标准要求
            target_compile_features(fuzz_parser PRIVATE cxx_std_11)

            # 添加包含目录
            target_include_directories(fuzz_parser
                PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/../include
            )

            # 添加模糊测试特定的编译选项
            target_compile_options(fuzz_parser PRIVATE -fsanitize=fuzzer-no-link)
            target_link_options(fuzz_parser PRIVATE -fsanitize=fuzzer)

            # 链接主项目和模糊测试库
            target_link_libraries(fuzz_parser
                PRIVATE
                sjkxq_json
            )

            # 添加模糊测试标签
            add_test(NAME fuzz_parser
                COMMAND fuzz_parser -max_total_time=60
            )

            set_tests_properties(fuzz_parser PROPERTIES
                LABELS "fuzz"
            )
            message(STATUS "Fuzz tests enabled (using Clang compiler)")
        else()
            message(WARNING "Fuzz tests are only supported with Clang compiler - disabling fuzz tests")
            set(BUILD_FUZZ_TESTS OFF CACHE BOOL "Disable fuzz tests for non-Clang compiler" FORCE)
        endif()
    endif()

    # 基准测试
if(BUILD_BENCHMARKS)
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
endif()