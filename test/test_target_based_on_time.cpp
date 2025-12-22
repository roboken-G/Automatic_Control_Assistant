#include <gtest/gtest.h>
#include "target_based_on_time.hpp"

// テストフィクスチャ（共通のセットアップを行うクラス）
class TargetBasedOnTimeTest : public ::testing::Test {
protected:
    aca::TargetBasedOnTime planner;
    aca::MotionProfile profile;

    void SetUp() override {
        // 各テスト実行前に呼ばれる
        profile.maxAcceleration = 100.0f;
        profile.maxVelocity = 50.0f;
    }
};

// Test 1: 正常系 (十分な距離)
TEST_F(TargetBasedOnTimeTest, NormalMove) {
    // 0 -> 1000
    planner.set(1000.0f, 0.0f, profile);
    
    // t=0
    auto status0 = planner.update(0);
    EXPECT_NEAR(status0.position, 0.0f, 0.001f);
    EXPECT_NEAR(status0.velocity, 0.0f, 0.001f);
    EXPECT_NEAR(status0.acceleration, 0.0f, 0.001f);

    // t=1000ms (加速中または等速)
    // 加速時間は約0.78sなので、1.0s時点では等速移動中のはず
    auto status1 = planner.update(1000); 
    EXPECT_NEAR(status1.velocity, 50.0f, 1.0f);
}

// Test 2: 所要時間の整合性チェック
TEST_F(TargetBasedOnTimeTest, TimeRequiredCheck) {
    planner.set(1000.0f, 0.0f, profile);
    float reqTime = planner.getTimeRequired();
    
    // 終了予定時刻を少し過ぎた時点で更新
    uint32_t checkTimeMs = static_cast<uint32_t>(reqTime * 1000) + 100;
    auto statusEnd = planner.update(checkTimeMs);
    
    EXPECT_NEAR(statusEnd.position, 1000.0f, 0.1f);
    EXPECT_NEAR(statusEnd.velocity, 0.0f, 0.1f);
    EXPECT_TRUE(statusEnd.isFinished);
}

// Test 3: 逆走 (負の方向への移動)
TEST_F(TargetBasedOnTimeTest, ReverseMove) {
    planner.set(-500.0f, 0.0f, profile);
    
    // 加速中のチェック
    auto statusRev = planner.update(500); 
    EXPECT_LT(statusRev.velocity, 0.0f) << "Velocity should be negative";
    EXPECT_LT(statusRev.position, 0.0f) << "Position should be negative";
}

// Test 4: 短距離移動 (最高速度に達しないケース)
TEST_F(TargetBasedOnTimeTest, ShortMove) {
    // 非常に短い距離
    planner.set(10.0f, 0.0f, profile);
    
    float reqTime = planner.getTimeRequired();
    auto statusEnd = planner.update(static_cast<uint32_t>(reqTime * 1000) + 50);
    
    EXPECT_NEAR(statusEnd.position, 10.0f, 0.01f);
    EXPECT_TRUE(statusEnd.isFinished);
}