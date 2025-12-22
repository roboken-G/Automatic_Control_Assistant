/**
 * @file simulate_target_based_on_time_pid.cpp
 * @brief TargetBasedOnTime + CascadePidController の統合シミュレーション
 * @details
 * S字軌道生成器で目標位置を時々刻々と生成し、
 * カスケードPID制御で追従させるシミュレーション。
 * DCモーター物理モデルを使用し、結果をCSVに出力します。
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

#include "target_based_on_time.hpp"
#include "CascadePidController.hpp"
#include "dc_motor_system.hpp"

int main() {
    // --- シミュレーション条件 ---
    const float sim_dt = 0.0001f;      // 0.1ms (物理演算周期)
    const float ctrl_dt = 0.005f;      // 5ms (制御周期)
    const float sim_time = 5.0f;       // 5秒間
    const int steps = static_cast<int>(sim_time / sim_dt);
    
    // 外乱設定 (重力トルク)
    const float gravity_torque = 5.0f; // [Nm]
    const float Kt = 0.5f;              // トルク定数 [Nm/A]
    const float gravity_ff_volts = gravity_torque / Kt; // 重力補償FF [V]

    // --- 軌道生成器の設定 ---
    aca::TargetBasedOnTime planner;
    aca::MotionProfile profile{
        .maxAcceleration = 200.0f,  // [rad/s^2]
        .maxVelocity = 30.0f,       // [rad/s]
        .firstVelocity = 0.0f,
        .finalVelocity = 0.0f
    };
    
    // 初期位置 0 → 目標位置 10 rad
    planner.set(10.0f, 0.0f, profile);
    float total_time_required = planner.getTimeRequired();
    std::cout << "[Planner] Time required: " << total_time_required << " s" << std::endl;

    // --- カスケードPID制御器の設定 ---
    aca::CascadeGain cascade_gain = {
        .pos_kp = 50.0f,      // 位置ループ比例ゲイン
        .vel_kp = 1.0f,       // 速度ループ比例ゲイン
        .vel_ki = 1.0f,       // 速度ループ積分ゲイン
        .max_speed = 35.0f,   // 最大速度 [rad/s]
        .max_output = 24.0f   // 最大出力電圧 [V]
    };
    aca::CascadePidController controller(cascade_gain);
    controller.set_stability_condition(0.01f, 0.5f); // 許容誤差 0.01 rad、持続時間 0.5s

    // --- 物理モデルのインスタンス ---
    DcMotorSystem motor;

    // --- CSV出力準備 ---
    std::ofstream ofs("build/simulation_data.csv");
    ofs << "time,target_pos,target_vel,actual_pos,actual_vel,control_output,error" << std::endl;
    ofs << std::fixed << std::setprecision(6);

    std::cout << "[Simulation] Starting..." << std::endl;

    // --- シミュレーションループ ---
    float control_timer = 0.0f;
    float control_output = 0.0f;
    float target_pos = 0.0f;
    float target_vel = 0.0f;
    
    for (int i = 0; i < steps; ++i) {
        float t = i * sim_dt;
        uint32_t time_ms = static_cast<uint32_t>(t * 1000.0f);

        // 制御周期タイマー
        control_timer += sim_dt;
        if (control_timer >= ctrl_dt) {
            control_timer = 0.0f;

            // 現在の状態取得
            float cur_pos = motor.get_position();
            float cur_vel = motor.get_velocity();

            // 軌道生成 (毎ステップ更新して目標値を取得)
            aca::ControlStatus trajectory = planner.update(time_ms);
            target_pos = trajectory.position;
            target_vel = trajectory.velocity;

            // カスケードPID制御実行 (FFあり)
            aca::RobotState state = {cur_pos, cur_vel};
            aca::FeedForward ff = {
                .ref_vel = target_vel,      // 目標速度FF
                .ref_acc = 0.0f,             // 加速度FF (今回は使用しない)
                .gravity = gravity_ff_volts  // 重力補償
            };

            aca::PidResult result = controller.update(target_pos, state, ff, 0.0f, ctrl_dt);
            control_output = result.output;
        }

        // 物理モデル更新 (外乱として重力トルクを加える)
        motor.update(control_output, gravity_torque, sim_dt);

        // CSV出力 (100Hz = 10ms間隔)
        if (i % 100 == 0) {
            float error = target_pos - motor.get_position();
            ofs << t << ","
                << target_pos << ","
                << target_vel << ","
                << motor.get_position() << ","
                << motor.get_velocity() << ","
                << control_output << ","
                << error << std::endl;
        }
    }

    std::cout << "[Simulation] Complete!" << std::endl;
    std::cout << "[Output] build/simulation_data.csv" << std::endl;

    ofs.close();
    return 0;
}
