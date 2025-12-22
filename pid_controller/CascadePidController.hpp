/**
 * @file CascadePidController.hpp
 * @brief 位置P-速度PIのカスケード制御クラス（構造体引数対応版）
 * @author Gemini
 * @date 2025-12-12
 */

#pragma once
#include "PidController.hpp"
#include <algorithm> // for std::clamp

namespace aca {

/**
 * @brief カスケード制御のゲイン設定構造体
 */
struct CascadeGain {
    float pos_kp;       ///< 位置ループ比例ゲイン
    float vel_kp;       ///< 速度ループ比例ゲイン
    float vel_ki;       ///< 速度ループ積分ゲイン
    float max_speed;    ///< 最大速度制限 (位置ループ上限)
    float max_output;   ///< 最大出力制限 (速度ループ上限)
};

/**
 * @brief フィードフォワード入力用構造体
 * @details 任意項目はデフォルト値 0.0f を持つため、
 * 必要な項目だけを指定して初期化できます。
 */
struct FeedForward {
    float ref_vel = 0.0f;     ///< 目標速度FF
    float ref_acc = 0.0f;     ///< 目標加速度FF (慣性補償用)
    float gravity = 0.0f;     ///< 重力補償FF
};

/**
 * @brief ロボットの現在状態構造体
 */
struct RobotState {
    float position;
    float velocity;
};

/**
 * @class CascadePidController
 * @brief 位置PIDと速度PIDを組み合わせたカスケード制御クラス
 */
class CascadePidController {
private:
    PidController pos_pid_; ///< 位置ループ (Outer Loop)
    PidController vel_pid_; ///< 速度ループ (Inner Loop)
    float max_output_;      ///< 最終出力のリミット値

public:
    /**
     * @brief コンストラクタ
     * @param gain 設定構造体
     */
    explicit CascadePidController(const CascadeGain& gain)
        : pos_pid_({ .kp = gain.pos_kp, .ki = 0.0f, .kd = 0.0f, 
                     .min_output = -gain.max_speed, .max_output = gain.max_speed }),
          vel_pid_({ .kp = gain.vel_kp, .ki = gain.vel_ki, .kd = 0.0f, 
                     .min_output = -gain.max_output, .max_output = gain.max_output }),
          max_output_(gain.max_output) // リミット値を保存
    {}

    /**
     * @brief ゲインを動的に変更する（自動調整用）
     * @param gain 新しいゲイン設定
     */
    void set_gains(const CascadeGain& gain) {
        // 位置PIDの設定更新
        pos_pid_.set_gains({gain.pos_kp, 0.0f, 0.0f, -gain.max_speed, gain.max_speed});
        // 速度PIDの設定更新
        vel_pid_.set_gains({gain.vel_kp, gain.vel_ki, 0.0f, -gain.max_output, gain.max_output});
        
        // リミット値なども必要なら更新
        max_output_ = gain.max_output;
    }

    void set_stability_condition(float threshold, float duration) {
        pos_pid_.set_stability_condition(threshold, duration);
    }

    void reset() {
        pos_pid_.reset();
        vel_pid_.reset();
    }

    /**
     * @brief 制御演算を行う (構造体渡し版)
     * @param target_pos 目標位置
     * @param state      現在のロボット状態 {position, velocity}
     * @param ff         フィードフォワード入力 {ref_vel, ref_acc, gravity}
     * @param inertia    慣性ゲイン (ma の m)
     * @param dt         経過時間 [s]
     */
    PidResult update(float target_pos, const RobotState& state, 
                     const FeedForward& ff, float inertia, float dt) 
    {
        // 1. 位置ループ
        PidResult pos_res = pos_pid_.update(target_pos, state.position, dt);

        // 2. 目標速度合成
        float target_vel_total = pos_res.output + ff.ref_vel;

        // 3. 速度ループ
        PidResult vel_res = vel_pid_.update(target_vel_total, state.velocity, dt);

        // 4. 物理FF加算
        float accel_ff_val = ff.ref_acc * inertia;
        float total_output = vel_res.output + accel_ff_val + ff.gravity;

        // FF加算後の最終出力に対してリミットをかける
        total_output = std::clamp(total_output, -max_output_, max_output_);

        return {total_output, pos_res.is_stable};
    }
    
    // 簡易版オーバーロード (FFなし)
    PidResult update(float target_pos, float cur_pos, float cur_vel, float dt) {
        RobotState state = {cur_pos, cur_vel};
        FeedForward ff = {}; // 全部ゼロ
        return update(target_pos, state, ff, 0.0f, dt);
    }
};

} // namespace aca