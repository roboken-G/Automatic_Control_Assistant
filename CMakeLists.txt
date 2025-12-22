cmake_minimum_required(VERSION 3.14)
project(PidControllerLibrary)

# C++20 (指定付き初期化子対応のため必須)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------
# 1. ライブラリ定義 (Header-only)
# --------------------------------------------------------
add_library(PidController INTERFACE)
target_include_directories(PidController INTERFACE pid_controller)

# --------------------------------------------------------
# 2. テスト設定 (Google Test)
# --------------------------------------------------------
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()

    # Google Test の自動ダウンロード設定
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # Windows環境での警告抑制
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    # ダウンロードと利用可能化
    FetchContent_MakeAvailable(googletest)

    # テスト実行ファイルの作成
    # testフォルダ内のすべての .cpp ファイルを取得
    file(GLOB TEST_SOURCES "test/*.cpp")
    
    # ライブラリ本体とGTestをリンク
    if(TEST_SOURCES)
        # すべてのテストコードを1つの実行ファイルにまとめる
        add_executable(run_tests ${TEST_SOURCES})
        target_link_libraries(run_tests PRIVATE PidController GTest::gtest_main)

        include(GoogleTest)
        gtest_discover_tests(run_tests)
    endif()
endif()

# --------------------------------------------------------
# 3. シミュレーション設定 (Generic Simulation Build)
# --------------------------------------------------------
option(BUILD_SIMULATION "Build simulation executable" ON)

if(BUILD_SIMULATION)
    # simulationディレクトリ内の .cpp ファイルをすべて取得
    file(GLOB SIMULATION_SOURCES "simulation/*.cpp")

    foreach(source_file ${SIMULATION_SOURCES})
        # ファイル名(拡張子なし)を取得 -> これをターゲット名(実行ファイル名)にする
        # 例: simulation/my_sim.cpp -> my_sim
        get_filename_component(target_name ${source_file} NAME_WE)

        # 実行ファイルの作成
        add_executable(${target_name} ${source_file})
        
        # ライブラリのリンク
        target_link_libraries(${target_name} PRIVATE PidController)
        
        message(STATUS "Added simulation target: ${target_name}")
    endforeach()
endif()