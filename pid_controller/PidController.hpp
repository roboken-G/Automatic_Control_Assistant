/**
 * @file PidController.hpp
 * @brief 標準的なPID制御クラス（構造体設定対応版）
 * @author Gemini
 * @date 2025-12-12
 */

#pragma once
#include <cmath>
#include <algorithm> // for std::clamp

namespace aca {

/**
 * @brief PIDゲインおよび制限値の設定構造体
 * @details 指示付き初期化子(.name = val)を使うことで、
 * 引数の順番間違いを防ぎ、可読性を向上させます。
 */
struct PidGain {
    float kp;               ///< 比例ゲイン
    float ki;               ///< 積分ゲイン
    float kd;               ///< 微分ゲイン
    float min_output;       ///< 出力下限
    float max_output;       ///< 出力上限
    float max_delta_output; ///< 出力変化量上限 (0以下で制限なし)
    float tau = 0.02f;      ///< D項フィルタ時定数 (デフォルト 0.02s)
};

/**
 * @brief 安定判定のための条件設定構造体
 */
struct StabilityCondition {
    float threshold; ///< 安定とみなす許容誤差の絶対値
    float duration;  ///< 許容誤差内に留まり続けるべき時間 [s]
};

/**
 * @brief PID計算の結果を返す構造体
 */
struct PidResult {
    /**
     * @brief 計算された操作量 (Output / Manipulated Variable)
     */
    float output;
    
    /**
     * @brief 安定状態かどうか
     */
    bool is_stable;
};

/**
 * @class PidController
 * @brief 位置型PID制御クラス
 */
class PidController {
private:
    PidGain gain_;          ///< ゲイン設定

    // --- 安定判定用 ---
    StabilityCondition stability_cond_;
    float stable_timer_;

    // --- 内部状態 ---
    float integral_;        ///< 積分値
    float prev_error_;      ///< 前回偏差
    float prev_d_term_;     ///< 前回D項
    float prev_output_;     ///< 前回出力
    float current_output_;  ///< 現在出力

public:
    /**
     * @brief コンストラクタ
     * @param gain 設定構造体
     */
    explicit PidController(const PidGain& gain)
        : gain_(gain),
          stability_cond_{0.0f, 0.0f}, stable_timer_(0.0f),
          integral_(0.0f), prev_error_(0.0f), prev_d_term_(0.0f),
          prev_output_(0.0f), current_output_(0.0f) {}

    /**
     * @brief 安定判定条件を設定する
     */
    void set_stability_condition(float threshold, float duration) {
        stability_cond_.threshold = threshold;
        stability_cond_.duration = duration;
    }

    void reset() {
        integral_ = 0.0f;
        prev_error_ = 0.0f;
        prev_d_term_ = 0.0f;
        prev_output_ = 0.0f;
        current_output_ = 0.0f;
        stable_timer_ = 0.0f;
    }

    void set_gains(const PidGain& gain) {
        gain_ = gain;
    }

    /**
     * @brief 制御演算を行う
     */
    PidResult update(float target, float measurement, float dt) {
        if (dt <= 0.0f) return {current_output_, false};

        float error = target - measurement;

        // --- 1. PID計算 ---
        float p_out = gain_.kp * error;
        float i_out = gain_.ki * (integral_ + error * dt); // 仮計算

        // D項 (フィルタ付き)
        float alpha = gain_.tau / (gain_.tau + dt);
        float d_term_raw = (error - prev_error_) / dt;
        float d_out = alpha * prev_d_term_ + (1.0f - alpha) * (d_term_raw * gain_.kd);

        float output = p_out + i_out + d_out;

        // --- 2. 出力制限 & 出力変化量制限 & アンチワインドアップ ---
        if (output > gain_.max_output) {
            output = gain_.max_output;
        } else if (output < gain_.min_output) {
            output = gain_.min_output;
        } else {
            integral_ += error * dt; // 飽和していない場合のみ積分更新
        }
        if (gain_.max_delta_output > 0.0f) {
            float delta_output = output - prev_output_;
            delta_output = std::clamp(delta_output, -gain_.max_delta_output, gain_.max_delta_output);
            output = prev_output_ + delta_output;
        }

        // --- 3. 状態更新 ---
        prev_error_ = error;
        prev_d_term_ = d_out;
        prev_output_ = current_output_;
        current_output_ = output;

        // --- 4. 安定判定 ---
        bool is_stable = false;
        if (stability_cond_.threshold > 0.0f) {
            if (std::abs(error) < stability_cond_.threshold) {
                stable_timer_ += dt;
            } else {
                stable_timer_ = 0.0f;
            }
            if (stable_timer_ >= stability_cond_.duration) {
                is_stable = true;
            }
        }

        return {output, is_stable};
    }

    float get_output() const { return current_output_; }
    float get_delta_output() const { return current_output_ - prev_output_; }
};

} // namespace aca