/**
 * @file simulate_comparison.cpp
 * @brief 標準PID vs カスケード制御 の比較シミュレーション
 * @details
 * 物理モデル(DCモーター + 昇降負荷)を用いて、
 * 1. Single Loop (位置PID)
 * 2. Cascade (位置P-速度PI + FF)
 * の挙動を比較し、結果をCSVに出力します。
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

#include "PidController.hpp"
#include "CascadePidController.hpp"
#include "dc_motor_system.hpp"

int main() {
    // --- シミュレーション条件 ---
    const float sim_dt = 0.0001f;      // 0.1ms (高精度)
    const float ctrl_dt = 0.005f;   // 5ms (制御周期)
    const float sim_time = 5.0f;   // 5秒間
    const int steps = (int)(sim_time / sim_dt);
    
    // 外乱設定
    const float gravity_torque = 5.0f;     // 常にかかる重力
    const float step_disturbance = 2.0f;   // 3秒後に追加される荷物

    // --- 制御器の準備 ---
    
    // 1. Single Loop PID (標準)
    // 重力に耐えるため積分(Ki)を強く設定
    aca::PidGain single_gain = {
        .kp = 20.0f, 
        .ki = 80.0f, 
        .kd = 1.0f,
        .min_output = -24.0f, 
        .max_output = 24.0f,
        .max_delta_output = 10.0f
    };
    aca::PidController single_pid(single_gain);

    // 2. Cascade PID (カスケード + FF)
    aca::CascadeGain cascade_gain = {
        .pos_kp = 50.0f,
        .vel_kp = 1.0f,
        .vel_ki = 1.0f,
        .max_speed = 30.0f,
        .max_output = 24.0f,
        .max_delta_output = 10.0f
    };
    aca::CascadePidController cascade_pid(cascade_gain);

    // 重力補償FF値の計算 (理論値: 電圧 = トルク / Kt)
    const float gravity_ff_volts = gravity_torque / 0.5f; // Kt=0.5

    // --- 物理モデルのインスタンス ---
    DcMotorSystem model_single;
    DcMotorSystem model_cascade;

    // --- CSV出力準備 ---
    std::ofstream ofs("build/simulation_result.csv");
    ofs << "time,target,pos_single,vol_single,pos_cascade,vol_cascade" << std::endl;
    ofs << std::fixed << std::setprecision(6);

    std::cout << "Simulation Start..." << std::endl;

    // --- ループ実行 ---
    float target_pos = 10.0f; // 目標位置 [rad]

    float control_timer = 0.0f;

    aca::PidResult res_s;
    aca::PidResult res_c;

    for (int i = 0; i < steps; ++i) {
        float t = i * sim_dt;
        
        // 外乱の計算 (重力 + 3秒後の追加負荷)
        float current_dist = gravity_torque;
        if (t >= 3.0f) {
            current_dist += step_disturbance;
        }

        control_timer += sim_dt;

        if (control_timer >= ctrl_dt) {
            control_timer = 0.0f;

            // -----------------------------------
            // 1. Single Loop PID 実行
            // -----------------------------------
            res_s = single_pid.update(target_pos, model_single.get_position(), ctrl_dt);

            // -----------  ------------------------
            // 2. Cascade PID 実行
            // -----------------------------------
            // 現在のロボット状態
            aca::RobotState state = {
                .position = model_cascade.get_position(),
                .velocity = model_cascade.get_velocity()
            };
            
            // フィードフォワード入力 (重力補償のみ有効化)
            aca::FeedForward ff = {
                .ref_vel = 0.0f,
                .ref_acc = 0.0f,
                .gravity = gravity_ff_volts // ここで重力を支える
            };

            res_c = cascade_pid.update(target_pos, state, ff, 0.0f, ctrl_dt);
        }

        // 1. single PID制御入力で物理モデル更新
        model_single.update(res_s.output, current_dist, sim_dt);
        // 2. cascade PID制御入力で物理モデル更新
        model_cascade.update(res_c.output, current_dist, sim_dt);

        // -----------------------------------
        // データ記録 (間引き: 10msごとに出力)
        // -----------------------------------
        if (i % 100 == 0) {
            ofs << t << "," 
                << target_pos << ","
                << model_single.get_position() << ","
                << res_s.output << ","
                << model_cascade.get_position() << ","
                << res_c.output << std::endl;
        }
    }

    std::cout << "Done. Result saved to simulation_result.csv" << std::endl;
    return 0;
}