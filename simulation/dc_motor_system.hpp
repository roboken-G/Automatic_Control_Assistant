/**
 * @file dc_motor_system.hpp
 * @brief DCモーターシステムの物理シミュレーション
 */

#pragma once

class DcMotorSystem {
private:
    // 物理パラメータ
    const float J  = 0.01f; // 慣性モーメント [kg*m^2]
    const float B  = 0.1f;  // 粘性摩擦係数 [N*m*s/rad]
    const float Kt = 0.5f;  // トルク定数 [N*m/A] (今回は簡易的に電圧->トルク変換係数とする)
    
    // 状態変数
    float position_ = 0.0f; // [rad]
    float velocity_ = 0.0f; // [rad/s]

public:
    void reset() {
        position_ = 0.0f;
        velocity_ = 0.0f;
    }

    /**
     * @brief 物理シミュレーションを1ステップ進める (オイラー法)
     * @param voltage 入力電圧 [V]
     * @param disturbance_torque 外乱トルク [N*m] (重力など)
     * @param dt 時間刻み [s]
     */
    void update(float voltage, float disturbance_torque, float dt) {
        // モーター発生トルク
        float motor_torque = voltage * Kt;
        
        // 運動方程式: J * acc = T_motor - B * vel - T_dist
        float acceleration = (motor_torque - (B * velocity_) - disturbance_torque) / J;

        // 積分
        velocity_ += acceleration * dt;
        position_ += velocity_ * dt;
    }

    float get_position() const { return position_; }
    float get_velocity() const { return velocity_; }
};