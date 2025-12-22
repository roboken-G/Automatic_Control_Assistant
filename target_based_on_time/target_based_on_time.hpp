/**
 * @file target_based_on_time.hpp
 * @brief 時間ベースの目標値生成クラスヘッダ
 */

#pragma once

#include <cmath>
#include <cfloat>
#include <cstdint>

namespace aca {

/**
 * @brief 動作パラメータ設定用構造体
 * @details set関数への引数渡し間違い（特に速度と加速度の順序）を防ぐために使用します．
 */
struct MotionProfile {
    float maxAcceleration;      ///< 最大加速度 [unit/s^2] (正の値)
    float maxVelocity;          ///< 最大速度 [unit/s] (正の値)
    float firstVelocity = 0.0f; ///< 初速度 [unit/s] (デフォルト: 0)
    float finalVelocity = 0.0f; ///< 終端速度 [unit/s] (デフォルト: 0)
};

/**
 * @brief 制御状態返却用構造体
 * @details update() の返り値として使用することで，同時刻の位置・速度・加速度を整合性を保って取得できます．
 */
struct ControlStatus {
    float position;     ///< 現在位置
    float velocity;     ///< 現在速度
    float acceleration; ///< 現在加速度
    bool isFinished;    ///< 目標に到達し，移動が完了したか
};

/**
 * @brief 時間ベースの目標値生成クラス (S字加減速)
 * @details 指定された加速度・速度制限に基づいて，現在時刻(time)における目標位置・速度・加速度を計算します．
 */
class TargetBasedOnTime {
public:
    /**
     * @brief コンストラクタ
     */
    TargetBasedOnTime() = default;

    /**
     * @brief 目標軌道の詳細設定
     * @param targetPosition 目標位置（絶対座標）
     * @param initialPosition 初期位置（絶対座標）
     * @param profile 動作パラメータ構造体 (最大加速度，最大速度など)
     */
    void set(float targetPosition, float initialPosition, const MotionProfile& profile);

    /**
     * @brief 目標軌道の設定 (現在位置0基準，距離指定)
     * @param targetPositionDistance 移動距離（相対距離）
     * @param profile 動作パラメータ構造体
     */
    void set(float targetPositionDistance, const MotionProfile& profile);

    /**
     * @brief 状態を更新し，最新の制御状態を返す
     * @param time_u32 経過時間 [ms]
     * @return ControlStatus 計算結果の構造体 (位置，速度，加速度，完了フラグ)
     */
    ControlStatus update(uint32_t time_u32);

    // --- インライン Getter (高速化のためヘッダに記述) ---

    /**
     * @brief 現在の計算済み位置を取得
     */
    float getPosition() const { return position; }

    /**
     * @brief 現在の計算済み速度を取得
     */
    float getVelocity() const { return velocity; }

    /**
     * @brief 現在の計算済み加速度を取得
     */
    float getAcceleration() const { return acceleration; }

    /**
     * @brief 目標到達にかかる所要時間を取得
     */
    float getTimeRequired() const {
        return periodOfAcceleration + periodOfConstantVelocity + periodOfDeceleration;
    }

private:
    // 設定値
    float targetPosition{0.0f};
    float initialPosition{0.0f};
    MotionProfile profile{}; // 設定値を保持
    float targetPositionDistance{0.0f};

    // 計算用内部パラメータ
    float maxVelocityThisTime{0.0f}; // 今回の移動で到達可能な最高速度
    float borderDistance{0.0f};      // 加速しきれる最小距離

    // 時間区分
    float periodOfAcceleration{0.0f};     // 加速時間
    float periodOfConstantVelocity{0.0f}; // 等速時間
    float periodOfDeceleration{0.0f};     // 減速時間

    // 現在の状態
    float position{0.0f};
    float velocity{0.0f};
    float acceleration{0.0f};

    // ヘルパー関数 (内部計算用)
    float getPositionBasic(float time, float bottomUp) const;
    float getVelocityBasic(float time, float bottomUp) const;
    float getAccelerationBasic(float time, float bottomUp) const;

    // ユーティリティ
    static inline float pow2(float x) { return x * x; }
};

} // namespace aca
