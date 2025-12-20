cmake_minimum_required(VERSION 3.14)
project(PidControllerLibrary)

# C++17標準 (ESP32やROS2で一般的)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------
# 1. ライブラリ定義 (Header-only)
# --------------------------------------------------------
# 実装ファイルを持たないインターフェースライブラリとして定義
add_library(PidController INTERFACE)

# includeディレクトリをインクルードパスとして公開
# ★修正: ディレクトリ名を include -> pid_controller に変更
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
      URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    # Windows環境での警告抑制
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    # ダウンロードと利用可能化
    FetchContent_MakeAvailable(googletest)

    # テスト実行ファイルの作成
    # ソースファイル名は test/test_pid.cpp を想定
    add_executable(run_tests test/test_pid.cpp)
    
    # ライブラリ本体とGTestをリンク
    target_link_libraries(run_tests PRIVATE PidController GTest::gtest_main)

    # ctest コマンドへの自動登録
    include(GoogleTest)
    gtest_discover_tests(run_tests)
endif()

# --------------------------------------------------------
# 3. シミュレーション設定 (Optional)
# --------------------------------------------------------
# シミュレーションコードがある場合のみビルド対象にする
option(BUILD_SIMULATION "Build simulation executable" ON)

if(BUILD_SIMULATION)
    # ★修正: ファイル名を simulate_comparison.cpp に合わせました
    set(SIM_SOURCE "simulation/simulate_comparison.cpp")
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SIM_SOURCE}")
        add_executable(run_simulation ${SIM_SOURCE})
        target_link_libraries(run_simulation PRIVATE PidController)
    endif()
endif()