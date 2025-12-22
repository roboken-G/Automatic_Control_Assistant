/**
 * @file test_pid.cpp
 * @brief Google Test を使用した単体テスト
 * @details CMake環境では unity.h ではなく gtest/gtest.h を使用します。
 */

#include <gtest/gtest.h>
#include "PidController.hpp"

// テストフィクスチャ (共通設定)
class PidTest : public ::testing::Test {
protected:
    const float EPSILON = 0.001f;
};

/**
 * @brief P制御の計算テスト
 */
TEST_F(PidTest, ProportionalCalculation) {
    // Arrange
    aca::PidGain gain = {
        .kp = 1.0f, .ki = 0.0f, .kd = 0.0f,
        .min_output = -100.0f, .max_output = 100.0f
    };
    aca::PidController pid(gain);
    
    // Act
    // 目標10, 現在0 -> 偏差10 -> 出力10
    auto res = pid.update(10.0f, 0.0f, 1.0f);

    // Assert
    EXPECT_NEAR(res.output, 10.0f, EPSILON);
}

/**
 * @brief 積分(I)制御のテスト
 */
TEST_F(PidTest, IntegralAccumulation) {
    aca::PidGain gain = {
        .kp = 0.0f, .ki = 1.0f, .kd = 0.0f,
        .min_output = -100.0f, .max_output = 100.0f
    };
    aca::PidController pid(gain);
    
    // 1回目
    auto res1 = pid.update(10.0f, 0.0f, 1.0f);
    EXPECT_NEAR(res1.output, 10.0f, EPSILON);
    
    // 2回目 (積分されて20になるはず)
    auto res2 = pid.update(10.0f, 0.0f, 1.0f);
    EXPECT_NEAR(res2.output, 20.0f, EPSILON);
}

/**
 * @brief アンチワインドアップ(飽和)テスト
 */
TEST_F(PidTest, Saturation) {
    float max_val = 5.0f;
    aca::PidGain gain = {
        .kp = 10.0f, .ki = 0.0f, .kd = 0.0f,
        .min_output = -max_val, .max_output = max_val
    };
    aca::PidController pid(gain);
    
    // 計算上は 10 * 10 = 100 だが、5.0 で止まるはず
    auto res = pid.update(10.0f, 0.0f, 0.1f);
    
    EXPECT_NEAR(res.output, max_val, EPSILON);
}

/**
 * @brief 差分出力(Delta)のテスト
 */
TEST_F(PidTest, DeltaOutput) {
    aca::PidGain gain = {
        .kp = 1.0f, .ki = 0.0f, .kd = 0.0f,
        .min_output = -100.0f, .max_output = 100.0f
    };
    aca::PidController pid(gain);
    
    // 1回目: output = 10
    pid.update(10.0f, 0.0f, 0.1f);
    
    // 2回目: output = 5
    pid.update(5.0f, 0.0f, 0.1f);
    
    // 差分: 5 - 10 = -5
    EXPECT_NEAR(pid.get_delta_output(), -5.0f, EPSILON);
}

// メイン関数はGTestが自動生成するものを使うため不要
// (CMakeLists.txt で GTest::gtest_main をリンクしているため)